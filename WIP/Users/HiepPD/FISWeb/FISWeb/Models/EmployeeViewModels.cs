using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace FISWeb.Models
{
    public class EmployeeViewModels
    {
        public bool[] monthAttend { get; set; }

        public int numDays { get; set; }

        public int countWk { get; set; }
        
        public double percenWk { get; set; }

        public int countWd { get; set; }

        public double percenWd { get; set; }

        public List<AttendViewModel> listAttent { get; set; }

        public EmployeeViewModels()
        {
            monthAttend = new bool[numDays];
            numDays = DateTime.DaysInMonth(DateTime.Now.Year, DateTime.Now.Month);
            countWk = 0;
            listAttent = new List<AttendViewModel>();
        }
    }
}
