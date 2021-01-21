using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using Newtonsoft.Json;

namespace NetFinal.Models
{
    [Table("student")]
    public partial class Student
    {
        public Student()
        {
            Course = new HashSet<Course>();
        }

        [Column("id")] public string Id { get; set; }
        [Required] [Column("name")] public string Name { get; set; }
        [Required] [Column("gender")] public string Gender { get; set; }
        [Required] [Column("class")] public string Class { get; set; }

        [JsonIgnore]
        [ForeignKey("Class")]
        [InverseProperty("Student")]
        public virtual Class ClassNavigation { get; set; }

        [JsonIgnore]
        [InverseProperty("IdNavigation")]
        public virtual ICollection<Course> Course { get; set; }
    }
}