using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace BindingHomework
{
    public partial class Student
    {
        [Key]
        [Column("student_id")]
        [StringLength(100)]
        public string StudentId { get; set; }

        [Column("name")]
        [StringLength(100)]
        public string Name { get; set; }

        [Column("avatar")]
        [StringLength(100)]
        public string Avatar { get; set; }

        [Column("major")]
        [StringLength(100)]
        public string Major { get; set; }

        [Column("class")]
        [StringLength(100)]
        public string Class { get; set; }

        [Column("sex")]
        [StringLength(100)]
        public string Sex { get; set; }

        [Column("age")]
        public int? Age { get; set; }

        [Column("regular_score")]
        public int? RegularScore { get; set; }

        [Column("final_exam_score")]
        public int? FinalExamScore { get; set; }
    }
}
