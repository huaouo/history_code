using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;
using Microsoft.AspNetCore.Routing;
using NetFinal.Services;

namespace NetFinal.Filters
{
    public class AuthFilterAttribute : ActionFilterAttribute
    {
        private readonly UserService _userService;

        public AuthFilterAttribute(UserService userService)
        {
            _userService = userService;
        }

        public override void OnActionExecuting(ActionExecutingContext context)
        {
            var controller = context.ActionDescriptor.RouteValues["controller"];
            var username = context.HttpContext.Session.GetString("username");
            var passwordHash = context.HttpContext.Session.GetString("passwordHash");
            if (controller != "Login" 
                && !_userService.ValidateByPasswordHash(username, passwordHash))
            {
                context.Result = new RedirectToRouteResult(
                    new RouteValueDictionary
                    {
                        {"controller", "Login"},
                        {"action", "Index"}
                    });
            }
        }
    }
}