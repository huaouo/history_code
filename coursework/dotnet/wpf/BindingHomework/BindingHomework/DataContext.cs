using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using BindingHomework.Annotations;

namespace BindingHomework
{
    public class DataContext : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        [NotifyPropertyChangedInvocator]
        public virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public List<string> Majors
        {
            get
            {
                using (var db = new DbModel())
                {
                    return (from t in db.Students
                        select t.Major).Distinct().ToList();
                }
            }
        }

        private string _selectedMajor;

        public string SelectedMajor
        {
            get => _selectedMajor;
            set
            {
                _selectedMajor = value;
                OnPropertyChanged(nameof(TreeNodes));
            }
        }

        public List<TreeNode> TreeNodes
        {
            get
            {
                using (var db = new DbModel())
                {
                    var treeNodes = new List<TreeNode>();

                    var classes = (from t in db.Students
                        where t.Major == SelectedMajor
                        select t.Class).Distinct().ToList();
                    foreach (var cls in classes)
                    {
                        var treeNode = new TreeNode
                        {
                            Name = cls,
                            Children = new List<TreeNode>()
                        };
                        (from t in db.Students
                            where t.Class == cls
                            select new {t.StudentId, t.Name}).Distinct().ToList().ForEach(
                            s => treeNode.Children.Add(new TreeNode
                            {
                                Name = s.Name,
                                Id = s.StudentId
                            })
                        );
                        treeNodes.Add(treeNode);
                    }

                    return treeNodes;
                }
            }
        }

        private TreeNode _selectedNode;

        public TreeNode SelectedNode
        {
            get => _selectedNode;
            set
            {
                _selectedNode = value;
                if (value.Children == null)
                {
                    OnPropertyChanged(nameof(Student));
                }
            }
        }

        public StudentViewModel Student
        {
            get
            {
                if (SelectedNode == null || SelectedNode.Children != null) return new StudentViewModel();
                using (var db = new DbModel())
                {
                    var studentViewModel = (from t in db.Students
                        where t.StudentId == SelectedNode.Id
                        select new StudentViewModel
                        {
                            StudentId = t.StudentId,
                            Name = t.Name,
                            Avatar = t.Avatar,
                            Major = t.Avatar,
                            Class = t.Class,
                            Sex = t.Sex,
                            Age = t.Age.ToString(),
                            RegularScore = t.RegularScore.ToString(),
                            FinalExamScore = t.FinalExamScore.ToString(),
                            TotalScore = (t.RegularScore + t.FinalExamScore).ToString(),
                        }).First();
                    studentViewModel.Source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                        ((Bitmap) Properties.Resources.ResourceManager.GetObject(studentViewModel.Avatar)).GetHbitmap(),
                        IntPtr.Zero,
                        Int32Rect.Empty,
                        BitmapSizeOptions.FromWidthAndHeight(70, 56));
                    return studentViewModel;
                }
            }
        }
    }

    public class StudentViewModel
    {
        public string StudentId { get; set; }
        public string Name { get; set; }
        public string Avatar { get; set; }
        public string Major { get; set; }
        public string Class { get; set; }
        public string Sex { get; set; }
        public string Age { get; set; }
        public string RegularScore { get; set; }
        public string FinalExamScore { get; set; }
        public string TotalScore { get; set; }
        public ImageSource Source { get; set; }
    }
}