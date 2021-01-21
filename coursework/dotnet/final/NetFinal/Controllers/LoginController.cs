using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using NetFinal.Services;

namespace NetFinal.Controllers
{
    public class LoginController : Controller
    {
        private readonly UserService _userService;

        public LoginController(UserService userService)
        {
            _userService = userService;
        }

        public IActionResult Index()
        {
            var username = HttpContext.Session.GetString("username");
            var passwordHash = HttpContext.Session.GetString("passwordHash");
            if (_userService.ValidateByPasswordHash(username, passwordHash))
            {
                return RedirectToAction("Index", "Final");
            }

            return View();
        }

        [HttpPost]
        public IActionResult Login()
        {
            var username = Request.Form["username"];
            var password = Request.Form["password"];
            if (_userService.ValidateByPassword(username, password))
            {
                HttpContext.Session.SetString("username", username);
                HttpContext.Session.SetString("passwordHash", _userService.GetPasswordHash(password));
                return RedirectToAction("Index", "Final");
            }

            ViewBag.LoginFail = true;
            return View("Index");
        }
    }
}