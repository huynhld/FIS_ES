using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace FISWeb.Models
{
    public class AdminViewModels
    {
        public User logUser { get; set; }

        public List<string> empListName { get; set; }

        public List<bool[]> monthAttend { get; set; }

        public int numDays { get; set; }

        public List<Device> listDevice { get; set; }

        public List<AttendViewModel> listAttent { get; set; }

        public AdminViewModels()
        {
            logUser = new User();
            empListName = new List<string>();
            monthAttend = new List<bool[]>();
            numDays = DateTime.DaysInMonth(DateTime.Now.Year, DateTime.Now.Month);
            listDevice = new List<Device>();
            listAttent = new List<AttendViewModel>();
        }
    }

    public class AttendViewModel
    {
        public string user_id;
        public string fullname;
        public string location;
        public DateTime log_time;
    }
}
