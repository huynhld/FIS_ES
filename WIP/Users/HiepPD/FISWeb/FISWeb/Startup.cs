using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(FISWeb.Startup))]
namespace FISWeb
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);
        }
    }
}
