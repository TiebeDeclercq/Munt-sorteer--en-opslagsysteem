namespace BLgip
{
    public partial class AppShell : Shell
    {
        public AppShell()
        {
            InitializeComponent();

            Routing.RegisterRoute(nameof(About), typeof(About));
            Routing.RegisterRoute(nameof(MainPage), typeof(MainPage));  
        }
    }
}
