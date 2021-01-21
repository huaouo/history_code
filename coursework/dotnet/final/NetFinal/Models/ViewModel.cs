using System.Collections.Generic;
using Microsoft.AspNetCore.Mvc.Rendering;

namespace NetFinal.Models
{
    public class ViewModel
    {
        public string SelectedCollege;
        public List<string> College;
        public Dictionary<string, List<string>> StudentName;
        public Student Student;
        public List<Course> Course;
    }
}