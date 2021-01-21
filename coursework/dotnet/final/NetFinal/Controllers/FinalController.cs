using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Rendering;
using NetFinal.Models;
using Newtonsoft.Json;

namespace NetFinal.Controllers
{
    static class SessionExtensions
    {
        public static void SetObject(this ISession session, string key, object value)
        {
            session.SetString(key, JsonConvert.SerializeObject(value));
        }

        public static T GetObject<T>(this ISession session, string key)
        {
            var value = session.GetString(key);
            return value == null ? default : JsonConvert.DeserializeObject<T>(value);
        }
    }

    public class FinalController : Controller
    {
        private readonly FinalContext _dbContext;

        public FinalController(FinalContext dbContext)
        {
            _dbContext = dbContext;
        }

        public IActionResult Index()
        {
            var viewModel = HttpContext.Session.GetObject<ViewModel>("ViewModel");
            if (viewModel == null)
            {
                viewModel = new ViewModel
                {
                    College = (from rec in _dbContext.Class
                        select rec.College).Distinct().ToList(),
                    StudentName = new Dictionary<string, List<string>>()
                };

                viewModel.SelectedCollege = viewModel.College[0];
                var classes = from rec in _dbContext.Class
                    where rec.College == viewModel.SelectedCollege
                    select rec.ClassName;
                foreach (var clazz in classes)
                {
                    viewModel.StudentName[clazz] = (from rec in _dbContext.Student
                        where rec.Class == clazz
                        select rec.Name).ToList();
                }

                var selectedClazz = viewModel.StudentName.Keys.First();
                viewModel.Student = (from rec in _dbContext.Student
                    where rec.Class == selectedClazz
                          && rec.Name == viewModel.StudentName[selectedClazz][0]
                    select rec).First();

                viewModel.Course = (from rec in _dbContext.Course
                    where rec.Id == viewModel.Student.Id
                    select rec).ToList();
                HttpContext.Session.SetObject("ViewModel", viewModel);
            }

            return View(viewModel);
        }

        [HttpPost]
        public IActionResult ChooseCollege()
        {
            string selectedCollege = Request.Form["selected-college"];
            var viewModel = HttpContext.Session.GetObject<ViewModel>("ViewModel");
            viewModel.SelectedCollege = selectedCollege;
            var classes = from rec in _dbContext.Class
                where rec.College == selectedCollege
                select rec.ClassName;
            viewModel.StudentName.Clear();
            foreach (var clazz in classes)
            {
                viewModel.StudentName[clazz] = (from rec in _dbContext.Student
                    where rec.Class == clazz
                    select rec.Name).ToList();
            }

            var selectedClazz = viewModel.StudentName.Keys.First();
            viewModel.Student = (from rec in _dbContext.Student
                where rec.Class == selectedClazz
                      && rec.Name == viewModel.StudentName[selectedClazz][0]
                select rec).First();

            viewModel.Course = (from rec in _dbContext.Course
                where rec.Id == viewModel.Student.Id
                select rec).ToList();
            HttpContext.Session.SetObject("ViewModel", viewModel);

            return RedirectToAction("Index");
        }

        public IActionResult ChooseStudent(string clazz, string name)
        {
            var viewModel = HttpContext.Session.GetObject<ViewModel>("ViewModel");
            viewModel.Student = (from rec in _dbContext.Student
                where rec.Class == clazz
                      && rec.Name == name
                select rec).First();
            viewModel.Course = (from rec in _dbContext.Course
                where rec.Id == viewModel.Student.Id
                select rec).ToList();
            HttpContext.Session.SetObject("ViewModel", viewModel);
            return RedirectToAction("Index");
        }

        public string TreeViewData()
        {
            var treeViewData = new List<Node>();
            var viewModel = HttpContext.Session.GetObject<ViewModel>("ViewModel");
            if (viewModel != null)
            {
                foreach (var key in viewModel.StudentName.Keys)
                {
                    var classNode = new Node
                    {
                        text = key,
                        href = "#",
                        nodes = new List<Node>()
                    };
                    foreach (var name in viewModel.StudentName[key])
                    {
                        classNode.nodes.Add(new Node
                        {
                            text = name,
                            href = $"/Final/ChooseStudent?clazz={key}&name={name}"
                        });
                    }

                    treeViewData.Add(classNode);
                }
            }

            return JsonConvert.SerializeObject(treeViewData);
        }

        [HttpPost]
        public IActionResult UpdateScore()
        {
            string id = Request.Form["id"];
            string courseName = Request.Form["course-name"];
            long score = long.Parse(Request.Form["score"]);

            var course = (from rec in _dbContext.Course
                where rec.Id == id && rec.CourseName == courseName
                select rec).First();
            _dbContext.Course.Attach(course);
            course.Score = score;
            _dbContext.Entry(course).Property(p => p.Score).IsModified = true;
            _dbContext.SaveChanges();

            var viewModel = HttpContext.Session.GetObject<ViewModel>("ViewModel");
            viewModel.Course = (from rec in _dbContext.Course
                where rec.Id == viewModel.Student.Id
                select rec).ToList();
            HttpContext.Session.SetObject("ViewModel", viewModel);

            return RedirectToAction("Index");
        }
    }
}