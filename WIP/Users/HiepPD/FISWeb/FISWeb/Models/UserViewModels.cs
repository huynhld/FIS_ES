using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace FISWeb.Models
{
    public class LoginUser
    {
        [Required]
        [Display(Name = "Username")]
        [EmailAddress]
        public string Username { get; set; }

        [Required]
        [DataType(DataType.Password)]
        [Display(Name = "Password")]
        public string Password { get; set; }

        [Display(Name = "Remember me?")]
        public bool RememberMe { get; set; }
    }

    public class Profile
    {
        public string position { get; set; }
        public string name { get; set; }
        public Nullable<System.DateTime> DOB { get; set; }
        public string phone { get; set; }
        public string department { get; set; }
        public string address { get; set; }
        public string email { get; set; }
    }
}
