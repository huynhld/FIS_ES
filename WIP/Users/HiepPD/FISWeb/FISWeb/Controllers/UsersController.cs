using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;
using FISWeb.Models;

namespace FISWeb.Controllers
{
    public class UsersController : Controller
    {
        private FISEntities db = new FISEntities();

        public ActionResult Login()
        {
            return View();
        }

        public ActionResult CheckLogin(LoginUser logUser)
        {
            List<User> list = db.Users.ToList();

            int ck = 0;
            User us = new User();

            foreach (var item in list)
            {
                if(item.username.Equals(logUser.Username) && item.password.Equals(logUser.Password))
                {
                    us = item;
                    ck = us.user_type ?? default(int);
                    break;
                }
            }

            if (ck == 0) return RedirectToAction("Login", "Users");
            else
            {
                Session["logUserID"] = us.user_id;
                Session["logUserType"] = us.user_type;
                Session["logUserName"] = us.full_name;
                if(ck == 1 || ck == 2) return RedirectToAction("Index", "Admin");
                else return RedirectToAction("Index", "Employee");
            }
        }

        public ActionResult Logout()
        {
            Session["logUserID"] = "";
            Session["logUserType"] = "";
            Session["logUserName"] = "";
            return RedirectToAction("Login", "Users");
        }

        // GET: Users
        public ActionResult Profile(string userID)
        {
            User logUser = new User();
            if(userID == null) logUser = db.Users.Find(Session["logUserID"]);
            else logUser = db.Users.Find(userID);
            Profile pr = new Profile();
            pr.position = db.Positions.Find(logUser.pos_id).pos_displayed;
            pr.name = logUser.full_name;
            pr.DOB = logUser.DOB;
            pr.phone = logUser.phone;
            pr.department = logUser.department;
            pr.address = logUser.address;
            pr.email = logUser.mail;

            return View(pr);
        }
    }
}
