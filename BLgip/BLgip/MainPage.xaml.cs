
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;
using InTheHand.Bluetooth;
using System.ComponentModel.Design;
using static Microsoft.Maui.ApplicationModel.Permissions;
using System.Text;
using System.Reflection.PortableExecutable;
using Microsoft.Maui.Layouts;
using Microsoft.Maui;
using System.Xml.Linq;

namespace BLgip
{
    public partial class MainPage : ContentPage
    {
        GattCharacteristic characteristicTx;
        RemoteGattServer gatt;
        PermissionStatus status;

        //Geldwaarden
        int cent5 = 0;
        int cent10 = 0;
        int cent20 = 0;
        int cent50 = 0;
        int eur1 = 0;
        int eur2 = 0;

        //Maximum geldwaarden
        double cent5Max = 20;
        double cent10Max = 20;
        double cent20Max = 20;
        double cent50Max = 20;
        double eur1Max = 20;
        double eur2Max = 20;

        //Toestand van de knoppen
        bool cent5Disabled = false;
        bool cent10Disabled = false;
        bool cent20Disabled = false;
        bool cent50Disabled = false;
        bool eur1Disabled = false;
        bool eur2Disabled = false;

        bool isMobile; //Draait het programma op een telefoon
        bool isIos; //Draait het programma op ios

        public MainPage()
        {
            InitializeComponent();
            setVisibility(false); //Dashboard niet zichtbaar zetten
            updateCoinValues(); 
            if (DeviceInfo.Current.Platform == DevicePlatform.Android || DeviceInfo.Current.Platform == DevicePlatform.iOS) //Kijken of het een mobiel apparaat is
            {
                if(DeviceInfo.Current.Platform == DevicePlatform.iOS)
                {
                    btnOverslaanVerbind.IsVisible = true; //Je kan het scherm overslaan omdat dit vereist is door Apple
                    isIos = true;
                }
                else
                {
                    btnOverslaanVerbind.IsVisible = false; //Indien geen Apple apparaat deze knop niet tonen
                    isIos = false;
                }
                isMobile = true;
            }
            else
            {
                isMobile = false;
            }
            
        }
        protected override async void OnAppearing() //Wanneer het programma opstart
        {
            base.OnAppearing();
            status = await Permissions.RequestAsync<InTheHand.Bluetooth.Permissions.Bluetooth>(); //Bluetooth toegang aanvragen
        }
        void setVisibility(bool visible) //Kies of de pagina getoont wordt
        {
            if(visible) //Alles tonen buiten de verbindknop
            {
                btnUitwerpen.IsEnabled = false;
                stklyUitwerpen.IsVisible = true;
                lblTotaalAantal.IsVisible = true;
                vtklyCoins.IsVisible = true;
                vtrklyInstellingen.IsVisible = true;
                btnVerbind.IsVisible = false;
                btnDisconnect.IsVisible = true;
                scrollView.IsEnabled = false;
                scrollView.IsEnabled = true;
                if(isIos)
                {
                    btnOverslaanVerbind.IsVisible = false;
                }
            }
            else //Niets tonen behalve de verbindknop
            {
                stklyUitwerpen.IsVisible = false;
                lblTotaalAantal.IsVisible = false;
                vtklyCoins.IsVisible = false;
                vtrklyInstellingen.IsVisible = false;
                debugMenu.IsVisible = false;
                btnDisconnect.IsVisible = false;
                btnVerbind.IsVisible = true;
                scrollView.IsEnabled = false;
                scrollView.IsEnabled = true;
                if (isIos)
                {
                    btnOverslaanVerbind.IsVisible = true;
                }
            }
            if(isIos) 
            {
                //Dit is een workaround voor een bug in maui.net op ios
                //Forceren om de scrollView opnieuw te berekenen
                scrollView.MaximumHeightRequest = 100;
                scrollView.MaximumHeightRequest = 2000;
            }
        }
        private void OnCounterClicked(object sender, EventArgs e) //Knop is ingedrukt om te verbinden met bluetooth
        {
            verbindenMetBluetooth(); //Verbinden met bluetooth
        }

        private async void verbindenMetBluetooth() //Verbinden met bluetooth
        {
            if (status == PermissionStatus.Granted) //Heb ik toegang tot bluetooth
            {
                InTheHand.Bluetooth.Bluetooth.AvailabilityChanged += Bluetooth_AvailabilityChanged;
                var device = await InTheHand.Bluetooth.Bluetooth.RequestDeviceAsync(new RequestDeviceOptions { AcceptAllDevices = true }); //Kies het bluetooth apparaat om met te verbinden
                if (device != null ) //Er is een apparaat gekozen
                {
                    gatt = device.Gatt;
                    device.GattServerDisconnected += Bluetooth_GattServerDisconnected;
                    lblConnectieStatus.Text = "Verbonden met: ";
                    lblConnectieStatusBold.Text = device.Name;
                    await gatt.ConnectAsync(); //Verbinden met de gatt 
                    var services = await gatt.GetPrimaryServicesAsync();
                    var service = services.FirstOrDefault(s => s.Uuid == Guid.Parse("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")); //De juiste service selecteren
                    if (service != null)
                    {
                        var characteristics = await service.GetCharacteristicsAsync();
                        characteristicTx = characteristics.FirstOrDefault(c => c.Uuid == Guid.Parse("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")); //Tx karakteristiek selecteren
                        var characteristicRx = characteristics.FirstOrDefault(c => c.Uuid == Guid.Parse("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")); //Rx karakteristiek selecteren
                        if (characteristicTx != null) //De tx karakteristiek bestaat
                        {
                            characteristicRx.CharacteristicValueChanged += Characteristic_CharacteristicValueChanged; //Abboneren om data te ontvangen
                            await characteristicRx.StartNotificationsAsync();
                            entryLCD.IsEnabled = false;
                            setVisibility(true); //Dashboard tonen
                            skipVerbinden(true); 
                            if(DeviceInfo.Current.Platform == DevicePlatform.WinUI)
                            {
                                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("v")); //Alle data opvragen van het muntsysteem
                            }
                            vibrate(500);
                        }
                    }
                }
                else
                {
                    lblConnectieStatus.Text = "Dit is niet het juiste apparaat";
                }
            }
            else
            {
                lblConnectieStatus.Text = "Ik heb geen rechten";
            }
        }
        private async void Bluetooth_AvailabilityChanged(object sender, EventArgs e)
        {
            var a = await InTheHand.Bluetooth.Bluetooth.GetAvailabilityAsync();
        }
        private async void Bluetooth_GattServerDisconnected(object sender, EventArgs e) //De bluetooth verbinding is verbroken
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                DisplayAlert("Bluetooth verbinding verloren", "De verbinding is weg", "OK"); //Alert tonen
                lblConnectieStatus.Text = "Verbinding verbroken";
                lblConnectieStatusBold.Text = "";
                setVisibility(false); //Terug naar het verbindscherm gaan
            });
            
        }
        private void Characteristic_CharacteristicValueChanged(object sender, GattCharacteristicValueChangedEventArgs e) //Er is data beschikbaar
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                var data = e.Value.ToArray(); //Ontvangen data omzetten naar een byte array
                var receivedText = Encoding.UTF8.GetString(data); //Byte array omzetten naar een string
                dataVerwerken(receivedText); //Deze data verwerken
            });
        } 
        private void vibrate(int ms) //Vibreren van je apparaat
        {
            if (isMobile) //Enkel op mobiele apparaten vibreren
            {
                Vibration.Default.Vibrate(ms);
            }
        }

        private void dataVerwerken(string data) //De data die serieel ontvangen is verwerken
        {
            lblOntvangenData.Text = data;
            if (data.Substring(0, 1) == "o") //Je ontvangt een nieuw geld aantal
            {
                if(data.Substring(0, 2) == "om") //Het is een maximaal aantal
                {
                    switch (data.Substring(2, data.LastIndexOf('c') - 1)) //Eerst het type van het geld bekijken 
                    {
                        case "5c":
                            cent5Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax5c.Text = "Max 5 cent: " + cent5Max;
                            break;
                        case "10c":
                            cent10Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax10c.Text = "Max 10 cent: " + cent10Max;
                            break;
                        case "20c":
                            cent20Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax20c.Text = "Max 20 cent: " + cent20Max;
                            break;
                        case "50c":
                            cent50Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax50c.Text = "Max 50 cent: " + cent50Max;
                            break;
                        case "100c":
                            eur1Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax1euro.Text = "Max 1 euro: " + eur1Max;
                            break;
                        case "200c":
                            eur2Max = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            lblMax2euro.Text = "Max 2 euro: " + eur2Max;
                            break;
                    }
                }
                else //Het is een gewoon geldaantal
                {
                    switch (data.Substring(1, data.LastIndexOf('c') - 1)) //Eerst het type van het geld bekijken 
                    {
                        case "5":
                            cent5 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                        case "10":
                            cent10 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                        case "20":
                            cent20 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                        case "50":
                            cent50 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                        case "100":
                            eur1 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                        case "200":
                            eur2 = int.Parse(data.Remove(0, data.LastIndexOf('c') + 1)); //Vervolgens het aantal van dit geldtype omzetten
                            break;
                    }
                }
            }
            updateCoinValues(); //Alles op je scherm updaten
        }

        void UpdateButtonState(Button button, ProgressBar progress, bool? isDisabled = null)
        {
            button.IsEnabled = progress.Progress > 0 && (isDisabled == null || !isDisabled.Value);
        }
        void UpdateLabelAndProgress(Label label, ProgressBar progressBar, int count, double maxCount, Label debugLabel = null)
        {
            label.Text = count + "x";
            progressBar.Progress = (float)count / maxCount;
            debugLabel.Text = count + "x";
        }

        private void updateCoinValues() //Alle waarden die getoont worden updaten
        {
            double total = (eur2 * 2) + (eur1 * 1) + (cent50 * 0.5) + (cent20 * 0.2) + (cent10 * 0.1) + (cent5 * 0.05); //Je totale bedrag berekenen
            lblTotaalAantal.Text = String.Format("{0:C}", total); //Totale bedrag tonen
            //De labels en progressbars updaten met het juiste geldtype
            UpdateLabelAndProgress(lblAantal5c, prgrs5c, cent5, cent5Max, lblAantal5cDebug);
            UpdateLabelAndProgress(lblAantal10c, prgrs10c, cent10, cent10Max, lblAantal10cDebug);
            UpdateLabelAndProgress(lblAantal20c, prgrs20c, cent20, cent20Max, lblAantal20cDebug);
            UpdateLabelAndProgress(lblAantal50c, prgrs50c, cent50, cent50Max, lblAantal50cDebug);
            UpdateLabelAndProgress(lblAantal1euro, prgrs1euro, eur1, eur1Max, lblAantal1euroDebug);
            UpdateLabelAndProgress(lblAantal2euro, prgrs2euro, eur2, eur2Max, lblAantal2euroDebug);
            //De knoppen updaten 
            UpdateButtonState(btnUitwerpen5c, prgrs5c, cent5Disabled);
            UpdateButtonState(btnUitwerpen10c, prgrs10c, cent10Disabled);
            UpdateButtonState(btnUitwerpen20c, prgrs20c, cent20Disabled);
            UpdateButtonState(btnUitwerpen50c, prgrs50c, cent50Disabled);
            UpdateButtonState(btnUitwerpen1euro, prgrs1euro, eur1Disabled);
            UpdateButtonState(btnUitwerpen2euro, prgrs2euro, eur2Disabled);
            UpdateButtonState(btn5cmin, prgrs5c);
            UpdateButtonState(btn10cmin, prgrs10c);
            UpdateButtonState(btn20cmin, prgrs20c);
            UpdateButtonState(btn50cmin, prgrs50c);
            UpdateButtonState(btn1eurmin, prgrs1euro);
            UpdateButtonState(btn2eurmin, prgrs2euro);
        }
        private async void verbindingVerbreken() //De verbinding met bluetooth verbreken
        {
            try
            {
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("lm0"));
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("lsVerbinding verbroken"));
                gatt.Disconnect();
            }
            catch { }
        }

        //Alle knoppen om geld uit te werpen
        private async void btnUitwerpen2euro_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s2")); //Verzenden van de data naar de Karakteristiek
            vibrate(50); //50ms vibreren
            eur2Disabled = true; 
            btnUitwerpen2euro.IsEnabled = false; //Knop uitzetten
            await Task.Delay(700); //700ms wachten
            eur2Disabled = false; //Knop terug aanzetten
            if (prgrs2euro.Progress > 0) //Enkel als je meer dan 0 hebt kan je de knop indrukken
            {
                btnUitwerpen2euro.IsEnabled = true;
            }
        }

        private async void btnUitwerpen1euro_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s1"));
            vibrate(50);
            eur1Disabled = true;
            btnUitwerpen1euro.IsEnabled = false;
            await Task.Delay(700);
            eur1Disabled = false;
            if (prgrs1euro.Progress > 0)
            {
                btnUitwerpen1euro.IsEnabled = true;
            }
        }

        private async void btnUitwerpen50c_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s50"));
            vibrate(50);
            cent50Disabled = true;
            btnUitwerpen50c.IsEnabled = false;
            await Task.Delay(700);
            cent50Disabled = false;
            if (prgrs50c.Progress > 0) {
                btnUitwerpen50c.IsEnabled = true;
            }
        }

        private async void btnUitwerpen20c_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s20"));
            vibrate(50);
            cent20Disabled = true;
            btnUitwerpen20c.IsEnabled = false;
            await Task.Delay(700);
            cent20Disabled = false;
            if (prgrs20c.Progress > 0)
            {
                btnUitwerpen20c.IsEnabled = true;
            }
        }

        private async void btnUitwerpen10c_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s10"));
            vibrate(50);
            cent10Disabled = true;
            btnUitwerpen10c.IsEnabled = false;
            await Task.Delay(700);
            cent10Disabled = false;
            if (prgrs10c.Progress > 0)
            {
                btnUitwerpen10c.IsEnabled = true;
            }
        }

        private async void btnUitwerpen5c_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("s5"));
            vibrate(50);
            cent5Disabled = true;
            btnUitwerpen5c.IsEnabled = false;
            await Task.Delay(700);
            cent5Disabled = false;
            if (prgrs5c.Progress > 0)
            {
                btnUitwerpen5c.IsEnabled = true;
            }
        }

        private async void chkLCD_CheckedChanged(object sender, CheckedChangedEventArgs e) //Checkbox lcd is verandert
        {
            if(chkLCD != null)
            {
                if (chkLCD.IsChecked) //Deze is ingeschakelt
                {
                    await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("l1")); //Dit versturen naar het muntsysteem
                    entryLCD.IsEnabled = true; //Eigen tekst schrijven aanleggen
                }
                else
                {
                    await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("l0"));
                    entryLCD.IsEnabled = false;
                }
            }
           
        }

        private async void chkNietsInlaten_CheckedChanged(object sender, CheckedChangedEventArgs e) //Checkbox doorlaten is verandert
        {
            if (chkNietsInlaten.IsChecked)
            {
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("d1"));
            }
            else
            {
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("d0"));
            }
        }

        private void chkDebug_CheckedChanged(object sender, CheckedChangedEventArgs e) //Checkbox debug is verandert
        {
            if (chkDebug.IsChecked)
            {
                debugMenu.IsVisible = true; //Je debugmenu zichtbaar maken
            }
            else
            {
                debugMenu.IsVisible = false;
            }
            if (isIos)
            {
                //Dit is een workaround voor een bug in maui.net op ios
                //Forceren om de scrollView opnieuw te berekenen
                scrollView.MaximumHeightRequest = 100;
                scrollView.MaximumHeightRequest = 2000;
            }
        }

        private async void btnDisconnect_Clicked(object sender, EventArgs e) //De knop om de verbinding te verbreken
        {
           verbindingVerbreken(); //De verbinding verbreken
           setVisibility(false); 
        }

        private async void btn2eurplus_Clicked(object sender, EventArgs e)
        {
            eur2++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o200c" + eur2));
        }

        private async void btn2eurmin_Clicked(object sender, EventArgs e)
        {
            eur2--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o200c" + eur2));
        }

        private async void btn1eurplus_Clicked(object sender, EventArgs e)
        {
            eur1++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o100c" + eur1));
        }

        private async void btn1eurmin_Clicked(object sender, EventArgs e)
        {
            eur1--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o100c" + eur1));
        }

        private async void btn50cplus_Clicked(object sender, EventArgs e)
        {
            cent50++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o50c" + cent50));
        }

        private async void btn50cmin_Clicked(object sender, EventArgs e)
        {
            cent50--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o50c" + cent50));
        }

        private async void btn20cplus_Clicked(object sender, EventArgs e)
        {
            cent20++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o20c" + cent20));
        }

        private async void btn20cmin_Clicked(object sender, EventArgs e)
        {
            cent20--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o20c" + cent20));
        }

        private async void btn10cplus_Clicked(object sender, EventArgs e)
        {
            cent10++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o10c" + cent10));
        }

        private async void btn10cmin_Clicked(object sender, EventArgs e)
        {
            cent10--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o10c" + cent10));
        }

        private async void btn5cplus_Clicked(object sender, EventArgs e)
        {
            cent5++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o5c" + cent5));
        }

        private async void btn5cmin_Clicked(object sender, EventArgs e)
        {
            cent5--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("o5c" + cent5));
        }

        private async void btnUitwerpen_Clicked(object sender, EventArgs e)
        {
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("u" + double.Parse(entryBedrag.Text) * 100));
        }

        private void entryBedrag_TextChanged(object sender, TextChangedEventArgs e)
        {
            double total = (eur2 * 2) + (eur1 * 1) + (cent50 * 0.5) + (cent20 * 0.2) + (cent10 * 0.1) + (cent5 * 0.05);
            if (double.TryParse(entryBedrag.Text, out double enteredValue))
            {
                if (enteredValue < total || string.IsNullOrWhiteSpace(entryBedrag.Text))
                {
                    btnUitwerpen.IsEnabled = true;
                }
                else
                {
                    btnUitwerpen.IsEnabled = false;
                }
            }
            else
            {
                btnUitwerpen.IsEnabled = false;
            }

        }

        private async void entryLCD_TextChanged(object sender, TextChangedEventArgs e)  //Er is tekst ingegeven om te tonen op de lcd
        {
            if(!string.IsNullOrWhiteSpace(entryLCD.Text)) //Het is werkelijk tekst
            {
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("ls" + entryLCD.Text)); //Dit sturen naar de lcd
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("lm1")); //Zorgen dat de lcd niets anders toont
            }
            else
            {
                await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("lm0")); //De lcd mag ook terug andere dingen tonen
            }
        }

        private async void btn5cMaxPlus_Clicked(object sender, EventArgs e)
        {
            cent5Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om5c" + cent5Max));
        }

        private async void btn5cMaxMin_Clicked(object sender, EventArgs e)
        {
            cent5Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om5c" + cent5Max));
        }

        private async void btn10cMaxPlus_Clicked(object sender, EventArgs e)
        {
            cent10Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om10c" + cent10Max));
        }

        private async void btn10cMaxMin_Clicked(object sender, EventArgs e)
        {
            cent10Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om10c" + cent10Max));
        }

        private async void btn20cMaxPlus_Clicked(object sender, EventArgs e)
        {
            cent20Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om20c" + cent20Max));
        }

        private async void btn20cMaxMin_Clicked(object sender, EventArgs e)
        {
            cent20Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om20c" + cent20Max));
        }

        private async void btn50cMaxPlus_Clicked(object sender, EventArgs e)
        {
            cent50Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om50c" + cent50Max));
        }

        private async void btn50cMaxMin_Clicked(object sender, EventArgs e)
        {
            cent50Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om50c" + cent50Max));
        }

        private async void btn1eurMaxPlus_Clicked(object sender, EventArgs e)
        {
            eur1Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om100c" + eur1Max));
        }

        private async void btn1eurMaxMin_Clicked(object sender, EventArgs e)
        {
            eur1Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om100c" + eur1Max));
        }

        private async void btn2euroMaxPlus_Clicked(object sender, EventArgs e)
        {
            eur2Max++;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om200c" + eur2Max));
        }

        private async void btn2euroMaxMin_Clicked(object sender, EventArgs e)
        {
            eur2Max--;
            await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("om200c" + eur2Max));
        }

        private void btnOverslaanVerbind_Clicked(object sender, EventArgs e)
        {
            setVisibility(true);
            skipVerbinden(false);
        }

        //Volgende functie is nodig om deze app beschikbaar te maken op de app store
        private void skipVerbinden(bool doThis) //Het verbinden overslaan maar je zal niets kunnen indrukken
        {
            if(!doThis) 
            {
                chkDebug.IsEnabled = false;
                chkLCD.IsEnabled = false;
                chkNietsInlaten.IsEnabled = false;
                entryLCD.IsEnabled = false;
            }
            else
            {
                chkDebug.IsEnabled = true;
                chkLCD.IsEnabled = true;
                chkNietsInlaten.IsEnabled = true;
                entryLCD.IsEnabled = true;
            }
        }

        private async void chkButtonsEnabled_CheckedChanged(object sender, CheckedChangedEventArgs e)  //Checkbox knoppen inschakelen is verandert
        {
            if(chkButtonsEnabled != null)
            {
                if (chkButtonsEnabled.IsChecked)
                {
                    await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("k1"));
                }
                else
                {
                    await characteristicTx.WriteValueWithoutResponseAsync(Encoding.UTF8.GetBytes("k0"));
                }
            }

        }
    }

}
