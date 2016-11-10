using FISWeb.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace FISWeb.Controllers
{
    public class EmployeeController : Controller
    {
        private FISEntities db = new FISEntities();

        // GET: Employee
        public ActionResult Index()
        {
            EmployeeViewModels model = new EmployeeViewModels();

            model.monthAttend = checkAttend(DateTime.Now.Month, model);
            string userID = Session["logUserID"].ToString();
            foreach (var item in db.Attents.Where(u => u.attent_user.Equals(userID)))
            {
                AttendViewModel at = new AttendViewModel();
                User us = db.Users.Find(item.attent_user);
                at.user_id = item.attent_user;
                at.fullname = us.full_name;
                at.log_time = item.attent_time;
                at.location = db.Devices.Find(item.attent_device).description;
                model.listAttent.Add(at);
            }

            model.countWd = model.monthAttend.Where(d => d == true).Count() - model.countWk;
            model.percenWk = (double) model.countWk / CountWeekend() * 100;
            model.percenWd = (double) model.countWd / (model.numDays - CountWeekend()) * 100;

            ViewBag.totalWd = model.numDays - CountWeekend();
            ViewBag.totalWk = CountWeekend();

            return View(model);
        }

        public bool[] checkAttend(int month, EmployeeViewModels model)
        {
            //listMonth is attend of user for a month (31 days)
            //1st == listMonth[1]
            bool[] listMonth = new bool[model.numDays + 1];

            string logUserID = Session["logUserID"].ToString();

            List<Attent> list = db.Attents.Where(o => o.attent_user.Equals(logUserID)).ToList();

            //save to bool[] 
            for (int i = 0; i < list.Count; i++)
            {
                int day = list[i].attent_time.Day;
                if (month == list[i].attent_time.Month)
                {
                    if (listMonth[day] == false && IsWeekend(list[i].attent_time)) model.countWk++;
                    listMonth[day] = true;
                }
            }
            return listMonth;
        }

        public static bool IsWeekend(DateTime date)
        {
            return new[] { DayOfWeek.Sunday, DayOfWeek.Saturday }.Contains(date.DayOfWeek);
        }

        public static int CountWeekend()
        {
            DateTime startDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, 1);
            DateTime endDate = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.DaysInMonth(DateTime.Now.Year, DateTime.Now.Month));

            int sundays = 0;
            int saturs = 0;

            for (DateTime dt = startDate; dt < endDate; dt = dt.AddDays(1.0))
            {
                if (dt.DayOfWeek == DayOfWeek.Sunday) sundays++;
                if (dt.DayOfWeek == DayOfWeek.Saturday) saturs++;
            }
            return sundays + saturs;
        }
    }
}