using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

using System.Xml;
using Xamarin.Forms.Xaml;

namespace SenzorMaster
{
    [DesignTimeVisible(false)]

    public partial class MainPage : ContentPage
    {
        private string _Temperatura = "Temperatura je: ?";
        public string Temperatura
        {
            get { return _Temperatura; }
            set
            {
                if (value == _Temperatura) return;
                _Temperatura = value;
                OnPropertyChanged();
            }
        }

        private string _Pokret = "Registrovan je pokret: ?";
        public string Pokret
        {
            get { return _Pokret; }
            set
            {
                if (value == _Pokret) return;
                _Pokret = value;
                OnPropertyChanged();
            }
        }

        private string _Svetlost = "Svetlost je: ?";
        public string Svetlost
        {
            get { return _Svetlost; }
            set
            {
                if (value == _Svetlost) return;
                _Svetlost = value;
                OnPropertyChanged();
            }
        }

        public MainPage()
        {
            
            InitializeComponent();
            this.BindingContext = this;
        }
        protected override void OnAppearing()
        {
           Device.StartTimer(new TimeSpan(0, 0, 0, 0, 500), Refresh);
           Refresh();
        }
        
        private bool Refresh()
        {
            Task.Run(async () =>
            {
                var response = await Connection.Instance.client.GetAsync(Connection.Instance.address);
                if (response.IsSuccessStatusCode)
                {
                    var responsedoc = new XmlDocument();
                    responsedoc.Load(await response.Content.ReadAsStreamAsync());
                    var root = responsedoc.DocumentElement;
                    Device.BeginInvokeOnMainThread(() =>
                    {
                        var temperatura  = (root.FirstChild.FirstChild as XmlText)?.Value ?? string.Empty;
                        var pokret = (root.FirstChild.NextSibling.FirstChild as XmlText)?.Value ?? string.Empty;
                        var svetlost = (root.FirstChild.NextSibling.NextSibling.FirstChild as XmlText)?.Value ?? string.Empty;

                        Temperatura = $"Temperatura je: {temperatura.Trim()}°C";
                        Pokret = $"Registrovan je pokret: {(pokret.Trim() == "1" ? "Da" : "Ne")}";
                        Svetlost = $"Svetlost: {(int.Parse(svetlost.Trim()) > 200 ? "Osvetljeno" : "Nije osvetljeno")}";
                    });
                }
            });
            return true;
        }

    }
}
