using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Http;


using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace SenzorMaster
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class ConnectionPage : ContentPage
    {
        public ConnectionPage()
        {
            InitializeComponent();
        }
        private async void Connect_Clicked(object sender, EventArgs e)
        {
            try
            {
                var address = $"http://{IPadresa.Text}:{Port.Text}";
                HttpClient client = new HttpClient();
                var response = await client.GetAsync(address);
                if (response.IsSuccessStatusCode)
                {
                    Connection.Instance.client = client;
                    Connection.Instance.address = address;
                    await Navigation.PushAsync(new MainPage());

                    await DisplayAlert("Konektovani", "Uspešno ste se konektovali!", "Ok");
                }
                else
                {
                    await DisplayAlert("Error", "Neuspešna konekcija!", "Ok");
                }
            }
            catch (Exception ex)
            {
                await DisplayAlert("Error", "" + ex.ToString(), "Ok");
            }
        }
    }
}