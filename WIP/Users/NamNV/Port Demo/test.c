
#define COM1 "/dev/ttyS0"
#define COM2 "/dev/ttyS1"
#define COM3 "/dev/ttyS2"
#define COM4 "/dev/ttyS3"

#define COM_PORT COM2
#define BAUD_RATE B115200
#define DATA_BUF_SIZE 10000

#include <stdio.h> // printf, getchar
#include <fcntl.h> // open, fcntl
#include <unistd.h> // close, read
#include <sys/ioctl.h> // ioctl
#include <termios.h> // termios struct

int main( void )
{
	int configure_com( int fd );

	unsigned char data[DATA_BUF_SIZE] = { 0 };
	int ret_val = 0;
	int cp_fd = 0;
	int byte_count = 0;
	int total_bytes = 0;
	struct timeval timeout;
	fd_set input;

	// Build the fd_set and set the timeout value used by select()
	FD_ZERO( &input ); // Clear the fd_set
	FD_SET( cp_fd, &input ); // The COM fd

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	// Open the com port device file.
	printf( "Opening device file.\n" );
	if( (cp_fd = open( COM_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK )) == -1 )
	{
		printf( "Unable to open device file %s.\n", COM_PORT );
		return( cp_fd );
	} // if( (cp_fd = open( COM_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK )) == -1 )

	// Setup the COM port.
	printf( "Configuring COM port.\n" );
	if( (ret_val = configure_com( cp_fd )) != 0 )
	{
		printf( "Failure configuring COM port.\n" );
		close( cp_fd );
		return( ret_val );
	} // if( (ret_val = configure_com( cp_fd )) != 0 )

	// Prompt the user to send data to the port. At this point, use another program
	// to send more than 4095 bytes of data to our com port. I typically send 100kb
	// or so.
	printf( "\n\nSend (a lot of) data now. When the send is complete, hit 'Enter'.\n" );
	while( (ret_val = getchar()) != '\n' );

	// Now, we're going to "flush" the com port buffer, and then query it for
	// the number of bytes available for reading after we flush it. In previous
	// tests, the 2nd query has reported 0 bytes available. However, a subsequent
	// read (even though we've stopped sending data) will return more data! It
	// should also be noted that a delay after the flush and before the 2nd query
	// will report more data available. It appears that a maximum of 4095 bytes
	// can be flushed per ioctl call.
	printf( "About to flush the buffer.\n" );

	if( (ret_val = ioctl( cp_fd, FIONREAD, &byte_count )) != 0 ) // Get number of bytes available.
	{
		printf( "Failure calling ioctl with FIONREAD argument.\n" );
		close( cp_fd );
		return( ret_val );
	} // if( (ret_val = ioctl( cp_fd, FIONREAD, &byte_count )) != 0 )

	while( byte_count > 0 )
	{
		// Note that this is 4095 bytes with versions 2.6.21.1 and 2.6.20,
		// but is 4096 bytes with version 2.6.17.14. Not sure if this is
		// related to the flush issue, but seemed worth noting.
		printf( "Bytes available before flush: %i\n", byte_count );
		ioctl( cp_fd, TCFLSH, TCIFLUSH );
		if( (ret_val = ioctl( cp_fd, TCFLSH, TCIFLUSH )) != 0 ) // Perform flush.
		{
		printf( "Failure calling ioctl with TCFLSH argument.\n" );
		close( cp_fd );
		return( ret_val );
		} // if( (ret_val = ioctl( cp_fd, TCFLSH, TCIFLUSH )) != 0 )

		// If we sleep for a bit here, the results of the next query will
		// be > 0. If we don't sleep, the results will be 0.

		//usleep( 100000 );

		if( (ret_val = ioctl( cp_fd, FIONREAD, &byte_count )) != 0 ) // Get number of bytes available.
		{
		printf( "Failure calling ioctl with FIONREAD argument.\n" );
		close( cp_fd );
		return( ret_val );
		} // if( (ret_val = ioctl( cp_fd, FIONREAD, &byte_count )) != 0 )
		printf( "Bytes available after flush: %i\n", byte_count );
	}

	printf( "Calling select(). Since we've already flushed the buffer, there\n" );
	printf( "should be NO MORE DATA available at the port!\n\n" );
	printf( "If nothing is available, select() will timeout after 5 seconds.\n" );

	// Build the fd set.
	FD_ZERO( &input ); // Clear the fd_set
	FD_SET( cp_fd, &input ); // The COM fd

	// Wait for up to 5 seconds for input with select. We would expect this to just
	// timeout, because there should be no more data available.
	if( (ret_val = select( cp_fd + 1, &input, NULL, NULL, &timeout )) < 0 )
		{
		printf( "Error on select().\n" );
		close( cp_fd );
		return( ret_val );
	} // if( (ret_val = select( cp_fd + 1, &input, NULL, NULL, NULL )) < 0 )

	else if( ret_val == 0 ) // Timeout. This is what should happen.
	{
		printf( "select() timed out. Nothing available at the port.\n" );
	} // else if( ret_val == 0 )

	// Data at the com port! Shouldn't be there, as we've "flushed" the buffer!
	// Note also that even though we're asking to read up to 10000 bytes per
	// read call, we can apparently only read 4095 bytes at a time.
	else if( FD_ISSET( cp_fd, &input ) )
	{
		printf( "select() triggered for COM port.\n" );
		printf( "This data should have been flushed already!\n" );
		ioctl( cp_fd, FIONREAD, &byte_count );
		printf( "Bytes available at port: %i\n", byte_count );
		while( (byte_count = read( cp_fd, data, DATA_BUF_SIZE )) > 0 )
		{
			printf( "Read %i bytes at COM port.\n", byte_count );
			total_bytes += byte_count;
		}
		printf( "Read %i bytes, AFTER flushing the buffer.\n", total_bytes );
	} // else if( FD_ISSET( cp_fd, &input ) )

	// Close device file.
	close( cp_fd );

	return( 0 );
} // int main( void )




/////////////////////////////////////////////////////////////////////////////
// configure_com()
// Takes a file descriptor for the COM port and configures it appropriately.
// Flushes the COM port buffer before returning. Returns 0 on
// success. If an error occurs, returns -1.
/////////////////////////////////////////////////////////////////////////////
int configure_com( int fd )
{
	struct termios options;
	int ret_val = 0;

	// Get attributes.
	if( (ret_val = tcgetattr( fd, &options )) == -1 )
	{
		printf( "configure_com: Unable to obtain COM port attributes.\n" );
		return( ret_val );
	} // if( (ret_val = tcgetattr( fd, &options )) == -1 )

	// Set input baud rate.
	if( (ret_val = cfsetispeed( &options, BAUD_RATE )) == -1 )
	{
		printf( "configure_com: Unable to set COM port input baud rate.\n" );
		return( ret_val );
	} // if( (ret_val = cfsetispeed( &options, BAUD_RATE )) == -1 )

	// Set output baud rate.
	if( (ret_val = cfsetospeed( &options, BAUD_RATE )) == -1 )
	{
		printf( "configure_com: Unable to set COM port output baud rate.\n" );
		return( ret_val );
	} // if( (ret_val = cfsetospeed( &options, BAUD_RATE )) == -1 )

	// Set up for 8N1, 8 data bits, no parity, 1 stop bit. Disable hardware flow
	// control. Ignore control signals, and enable the port for reading.
	options.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
	options.c_cflag |= (CS8 | CLOCAL | CREAD);

	// Set up for raw (non-canonical) input, disable echoing and signalling.
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	// Disable software flow control
	options.c_iflag &= ~(IXON | IXOFF | IXANY);

	// Set up for raw output.
	options.c_oflag &= ~OPOST;

	// Set the updated attributes, flushing the port first.
	if( (ret_val = tcsetattr( fd, TCSAFLUSH, &options )) == -1 )
	{
		printf( "configure_com: Unable to set COM port attributes.\n" );
		return( ret_val );
	} // if( (ret_val = tcsetattr( fd, TCSAFLUSH, &options )) == -1 )

	// Ensure we're setup for non-blocking operations. If open was used
	// with O_NONBLOCK or O_NDELAY, this will be the case anyway. This fcntl
	// call will make sure of this.
	if( (ret_val = fcntl( fd, F_SETFL, FNDELAY )) == -1 )
	{
		printf( "configure_com: Unable to configure COM port for non-blocking operations.\n" );
		return( ret_val );
	} // if( (ret_val = fcntl( fd, F_SETFL, FNDELAY )) == -1 )

	return( ret_val );
} // static int configure_com( int cp_fd )
