using System;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Metadata;

namespace NetFinal.Models
{
    public partial class FinalContext : DbContext
    {
        public FinalContext()
        {
        }

        public FinalContext(DbContextOptions<FinalContext> options)
            : base(options)
        {
        }

        public virtual DbSet<Class> Class { get; set; }
        public virtual DbSet<Course> Course { get; set; }
        public virtual DbSet<Student> Student { get; set; }
        public virtual DbSet<User> User { get; set; }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.HasAnnotation("ProductVersion", "2.2.4-servicing-10062");

            modelBuilder.Entity<Class>(entity => { entity.Property(e => e.ClassName).ValueGeneratedNever(); });

            modelBuilder.Entity<Course>(entity =>
            {
                entity.HasKey(e => new {e.Id, Course1 = e.CourseName});

                entity.HasOne(d => d.IdNavigation)
                    .WithMany(p => p.Course)
                    .HasForeignKey(d => d.Id)
                    .OnDelete(DeleteBehavior.ClientSetNull);
            });

            modelBuilder.Entity<Student>(entity =>
            {
                entity.Property(e => e.Id).ValueGeneratedNever();

                entity.HasOne(d => d.ClassNavigation)
                    .WithMany(p => p.Student)
                    .HasForeignKey(d => d.Class)
                    .OnDelete(DeleteBehavior.ClientSetNull);
            });

            modelBuilder.Entity<User>(entity => { entity.Property(e => e.Id).ValueGeneratedNever(); });
        }
    }
}