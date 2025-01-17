#include <iostream>
#include <string>
#include <CaribouLite.hpp>
#include <thread>
#include <complex>

// Print Board Information
void printInfo(CaribouLite& cl)
{
    std::cout << "Initialized CaribouLite: " << cl.IsInitialized() << std::endl;
    std::cout << "API Versions: " << cl.GetApiVersion() << std::endl;
    std::cout << "Hardware Serial Number: " << std::hex << cl.GetHwSerialNumber() << std::endl;
    std::cout << "System Type: " << cl.GetSystemVersionStr() << std::endl;
    std::cout << "Hardware Unique ID: " << cl.GetHwGuid() << std::endl;
}

// Detect the board before instantiating it
void detectBoard()
{
    CaribouLite::SysVersion ver;
    std::string name;
    std::string guid;
    
    if (CaribouLite::DetectBoard(&ver, name, guid))
    {
        std::cout << "Detected Version: " << CaribouLite::GetSystemVersionStr(ver) << ", Name: " << name << ", GUID: " << guid << std::endl;
    }
    else
    {
        std::cout << "Undetected CaribouLite!" << std::endl;
    }    
}

// Rx Callback (async)
void receivedSamples(CaribouLiteRadio* radio, const std::complex<float>* samples, CaribouLiteMeta* sync, size_t num_samples)
{
    std::cout << "Radio: " << radio->GetRadioName() << " Received " << std::dec << num_samples << " samples" << std::endl;
    
}


// Main entry
int main ()
{
    // try detecting the board before getting the instance
    detectBoard();
    
    // get driver instance - use "CaribouLite&" rather than "CaribouLite" (ref)
    CaribouLite &cl = CaribouLite::GetInstance();
    
    // print the info after connecting
    printInfo(cl);
        
    // get the radios
    CaribouLiteRadio *s1g = cl.GetRadioChannel(CaribouLiteRadio::RadioType::S1G);
    CaribouLiteRadio *hif = cl.GetRadioChannel(CaribouLiteRadio::RadioType::HiF);
    
    // write radio information
    std::cout << "First Radio Name: " << s1g->GetRadioName() << "  MtuSize: " << std::dec << s1g->GetNativeMtuSample() << " Samples" << std::endl;
    std::cout << "First Radio Name: " << hif->GetRadioName() << "  MtuSize: " << std::dec << hif->GetNativeMtuSample() << " Samples" << std::endl;
    
    std::vector<CaribouLiteFreqRange> range_s1g = s1g->GetFrequencyRange();
    std::vector<CaribouLiteFreqRange> range_hif = hif->GetFrequencyRange();
    std::cout << "S1G Frequency Regions:" << std::endl;
    for (int i = 0; i < range_s1g.size(); i++)
    {
        std::cout << "  " << i << ": " << range_s1g[i] << std::endl;
    }
    
    std::cout << "HiF Frequency Regions:" << std::endl;
    for (int i = 0; i < range_hif.size(); i++)
    {
        std::cout << "  " << i << ": " << range_hif[i] << std::endl;
    }
    
    // start receiving until enter pressed on 900MHz
    int num = 1;
    while (num --)
    {
        try
        {
            s1g->SetFrequency(900000000);
        }
        catch (...)
        {
            std::cout << "The specified freq couldn't be used" << std::endl;
        }
        
        s1g->SetRxGain(50);
        s1g->SetAgc(false);
        s1g->StartReceiving(receivedSamples);

        getchar();
        
        
        try
        {
            hif->SetFrequency(2400000000);
        }
        catch (...)
        {
            std::cout << "The specified freq couldn't be used" << std::endl;
        }
        hif->SetRxGain(50);
        hif->SetAgc(false);
        hif->StartReceiving(receivedSamples, 20000);
        
        getchar();
    }
    
    hif->StopReceiving();
    
    return 0;
}
