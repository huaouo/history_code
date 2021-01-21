using System;
using System.Drawing;
using System.Windows;
using System.Windows.Media.Imaging;

namespace BindingHomework
{
    public partial class MainWindow : Window
    {
        private readonly DataContext _context;
        public MainWindow()
        {
            InitializeComponent();
            _context = (DataContext)DataContext;
        }

        private void TreeView_OnSelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            _context.SelectedNode = (TreeNode)TreeView.SelectedItem;

//            if (_context.SelectedNode != null && _context.SelectedNode.Children == null &&_context.Student != null)
//            {
//                Avatar.Source = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
//                    ((Bitmap)Properties.Resources.ResourceManager.GetObject(_context.Student.Avatar)).GetHbitmap(),
//                    IntPtr.Zero,
//                    Int32Rect.Empty,
//                    BitmapSizeOptions.FromWidthAndHeight(70, 56));
//            }
        }
    }
}