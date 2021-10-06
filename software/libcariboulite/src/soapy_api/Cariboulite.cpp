#include <math.h>
#include "Cariboulite.hpp"
#include "cariboulite_config/cariboulite_config_default.h"

//========================================================
int soapy_sighandler(int signum)
{
    SoapySDR_logf(SOAPY_SDR_DEBUG, "Received signal %d", signum);
    switch (signum)
    {
        case SIGINT: printf("Caught SIGINT\n"); break;
        case SIGTERM: printf("Caught SIGTERM\n"); break;
        case SIGABRT: printf("Caught SIGABRT\n"); break;
        case SIGILL: printf("Caught SIGILL\n"); break;
        case SIGSEGV: printf("Caught SIGSEGV\n"); break;
        case SIGFPE:  printf("Caught SIGFPE\n"); break;
        default: printf("Caught Unknown Signal %d\n", signum); return -1; break;
    }

    return -1;
}

/*******************************************************************
 * Constructor / Destructor
 ******************************************************************/
Cariboulite::Cariboulite(const SoapySDR::Kwargs &args)
{
    CARIBOULITE_CONFIG_DEFAULT(temp);
    memcpy(&cariboulite_sys, &temp, sizeof(cariboulite_sys));
    cariboulite_init_driver(&cariboulite_sys, (void*)soapy_sighandler, NULL);
    cariboulite_init_radios(&radios, &cariboulite_sys);
    // TODO: Exception when error
}

//========================================================
Cariboulite::~Cariboulite()
{
    printf("~Cariboulite\n");
    cariboulite_dispose_radios(&radios);
    cariboulite_release_driver(&cariboulite_sys);
}

/*******************************************************************
 * Identification API
 ******************************************************************/
SoapySDR::Kwargs Cariboulite::getHardwareInfo() const
{
    printf("getHardwareInfo\n");
    //key/value pairs for any useful information
    //this also gets printed in --probe
    SoapySDR::Kwargs args;

    uint32_t serial_number = 0;
    uint32_t deviceId = 0;
    int count = 0;
    cariboulite_get_serial_number((cariboulite_st*)&cariboulite_sys, &serial_number, &count) ;

    args["device_id"] = std::to_string(deviceId);
    args["serial_number"] = std::to_string(serial_number);
    args["hardware_revision"] = cariboulite_sys.board_info.product_version;
    args["fpga_revision"] = std::to_string(1);
    args["vendor_name"] = cariboulite_sys.board_info.product_vendor;
    args["product_name"] = cariboulite_sys.board_info.product_name;

    return args;
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
std::vector<std::string> Cariboulite::listAntennas( const int direction, const size_t channel ) const
{
    printf("listAntennas\n");
	std::vector<std::string> options;
    if (channel == cariboulite_channel_s1g) options.push_back( "TX/RX Sub1GHz" );
    else if (channel == cariboulite_channel_6g) options.push_back( "TX/RX 6GHz" );
    
	return(options);
}

//========================================================
std::string Cariboulite::getAntenna( const int direction, const size_t channel ) const
{
    printf("getAntenna\n");
	if (channel == cariboulite_channel_s1g) return "TX/RX Sub1GHz";
    else if (channel == cariboulite_channel_6g) return "TX/RX 6GHz";
    return "";
}

/*******************************************************************
 * Gain API
 ******************************************************************/
std::vector<std::string> Cariboulite::listGains(const int direction, const size_t channel) const
{
    std::vector<std::string> gains;
    if (direction == SOAPY_SDR_RX)
    {
        for (int i = 0; i < 23; i ++)
        {
            char t[10] = {0};
            sprintf(t, "%d dB", i*3);
            gains.push_back(t);
        }
    }
    else if (direction == SOAPY_SDR_TX)
    {
        for (int i = 0; i < 32; i ++)
        {
            char t[10] = {0};
            sprintf(t, "%d dB", i);
            gains.push_back(t);
        }
    }

    return (gains);
}

//========================================================
/*!
* Set the overall amplification in a chain.
* The gain will be distributed automatically across available element.
* \param direction the channel direction RX or TX
* \param channel an available channel on the device
* \param value the new amplification value in dB
*/
void Cariboulite::setGain(const int direction, const size_t channel, const double value)
{
    cariboulite_radio_state_st* rad = channel == cariboulite_channel_s1g? &radios.radio_sub1g : &radios.radio_6g;
    bool cur_agc_mode = rad->rx_agc_on;

    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_set_rx_gain_control(&radios, (cariboulite_channel_en)channel, cur_agc_mode, value);
    }
    else if (direction == SOAPY_SDR_TX)
    {
        // base if -18dBm output so, given a gain of 0dB we should have -18 dBm
        cariboulite_set_tx_power(&radios, (cariboulite_channel_en)channel, value - 18.0);
    }
}

//========================================================
void Cariboulite::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
    // no amplification device names - redirect to  the regular setGain function
    Cariboulite::setGain(direction, channel, value);
}

//========================================================
double Cariboulite::getGain(const int direction, const size_t channel) const
{
    int value = 0;
    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_get_rx_gain_control((cariboulite_radios_st*)&radios, (cariboulite_channel_en)channel, NULL, &value);
    }
    else if (direction == SOAPY_SDR_TX)
    {
        int temp = 0;
        cariboulite_get_tx_power((cariboulite_radios_st*)&radios, (cariboulite_channel_en)channel, &temp);
        value = temp + 18.0;
    }
    return value;
}

//========================================================
double Cariboulite::getGain(const int direction, const size_t channel, const std::string &name) const
{
    return getGain(direction, channel);
}

//========================================================
SoapySDR::Range Cariboulite::getGainRange(const int direction, const size_t channel) const
{
    if (direction == SOAPY_SDR_RX)
    {
        return SoapySDR::Range(0.0, 23.0*3.0);
    }

    // and for TX
    return SoapySDR::Range(0.0, 31.0);
}

//========================================================
SoapySDR::Range Cariboulite::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    return getGainRange(direction, channel);
}


//========================================================
bool Cariboulite::hasGainMode(const int direction, const size_t channel) const
{
    return (direction==SOAPY_SDR_RX?true:false);
}

//========================================================
/*!
* Set the automatic gain mode on the chain.
* \param direction the channel direction RX or TX
* \param channel an available channel on the device
* \param automatic true for automatic gain setting
*/
void Cariboulite::setGainMode( const int direction, const size_t channel, const bool automatic )
{
    printf("setGainMode\n");
    cariboulite_radio_state_st* rad = channel == cariboulite_channel_s1g? &radios.radio_sub1g : &radios.radio_6g;
    bool rx_gain = rad->rx_gain_value_db;

    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_set_rx_gain_control(&radios, (cariboulite_channel_en)channel, automatic, rx_gain);
    }
}

//========================================================
/*!
* Get the automatic gain mode on the chain.
* \param direction the channel direction RX or TX
* \param channel an available channel on the device
* \return true for automatic gain setting
*/
bool Cariboulite::getGainMode( const int direction, const size_t channel ) const
{
    printf("getGainMode\n");
    bool mode = false;

    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_get_rx_gain_control((cariboulite_radios_st*)&radios, (cariboulite_channel_en)channel, 
                                            &mode, NULL);
        return mode;
    }

    return (false);
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/
void Cariboulite::setSampleRate( const int direction, const size_t channel, const double rate )
{
    cariboulite_radio_state_st* rad = channel == cariboulite_channel_s1g? &radios.radio_sub1g : &radios.radio_6g;
    at86rf215_radio_f_cut_en rx_cuttof = rad->rx_fcut;
    at86rf215_radio_f_cut_en tx_cuttof = rad->tx_fcut;

    printf("setSampleRate\n");
    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_set_rx_samp_cutoff(&radios, 
                                   (cariboulite_channel_en)channel,
                                   at86rf215_radio_rx_sample_rate_4000khz,
                                   rx_cuttof);
    }
    else if (direction == SOAPY_SDR_TX)
    {
        cariboulite_set_tx_samp_cutoff(&radios, 
                                   (cariboulite_channel_en)channel,
                                   at86rf215_radio_rx_sample_rate_4000khz,
                                   tx_cuttof);
    }
}

//========================================================
double Cariboulite::getSampleRate( const int direction, const size_t channel ) const
{
    printf("getSampleRate\n");
    // A single option
    return 4000000;
}

//========================================================
std::vector<double> Cariboulite::listSampleRates( const int direction, const size_t channel ) const
{
    printf("listSampleRates\n");
    std::vector<double> options;
	options.push_back( 4000000 );
    /*options.push_back( 2000000 ); // we want currently to allow only 4 MSPS to make the FPGA implementation easier
    options.push_back( 1333000 );
    options.push_back( 1000000 );
    options.push_back( 800000 );
    options.push_back( 666000 );
    options.push_back( 500000 );
    options.push_back( 400000 );*/
	return(options);
}

//========================================================
static at86rf215_radio_rx_bw_en convertRxBandwidth(double bw_numeric)
{
    if (fabs(bw_numeric - 160000) < 1) return at86rf215_radio_rx_bw_BW160KHZ_IF250KHZ;
    if (fabs(bw_numeric - 200000) < 1) return at86rf215_radio_rx_bw_BW200KHZ_IF250KHZ;
    if (fabs(bw_numeric - 250000) < 1) return at86rf215_radio_rx_bw_BW250KHZ_IF250KHZ;
    if (fabs(bw_numeric - 320000) < 1) return at86rf215_radio_rx_bw_BW320KHZ_IF500KHZ;
    if (fabs(bw_numeric - 400000) < 1) return at86rf215_radio_rx_bw_BW400KHZ_IF500KHZ;
    if (fabs(bw_numeric - 500000) < 1) return at86rf215_radio_rx_bw_BW500KHZ_IF500KHZ;
    if (fabs(bw_numeric - 630000) < 1) return at86rf215_radio_rx_bw_BW630KHZ_IF1000KHZ;
    if (fabs(bw_numeric - 800000) < 1) return at86rf215_radio_rx_bw_BW800KHZ_IF1000KHZ;
    if (fabs(bw_numeric - 1000000) < 1) return at86rf215_radio_rx_bw_BW1000KHZ_IF1000KHZ;
    if (fabs(bw_numeric - 1250000) < 1) return at86rf215_radio_rx_bw_BW1250KHZ_IF2000KHZ;
    if (fabs(bw_numeric - 1600000) < 1) return at86rf215_radio_rx_bw_BW1600KHZ_IF2000KHZ;
    if (fabs(bw_numeric - 2000000) < 1) return at86rf215_radio_rx_bw_BW2000KHZ_IF2000KHZ;
    return at86rf215_radio_rx_bw_BW2000KHZ_IF2000KHZ;
}

//========================================================
static double convertRxBandwidth(at86rf215_radio_rx_bw_en bw_en)
{
    if (at86rf215_radio_rx_bw_BW160KHZ_IF250KHZ == bw_en) return 160000;
    if (at86rf215_radio_rx_bw_BW200KHZ_IF250KHZ == bw_en) return 200000;
    if (at86rf215_radio_rx_bw_BW250KHZ_IF250KHZ == bw_en) return 250000;
    if (at86rf215_radio_rx_bw_BW320KHZ_IF500KHZ == bw_en) return 320000;
    if (at86rf215_radio_rx_bw_BW400KHZ_IF500KHZ == bw_en) return 400000;
    if (at86rf215_radio_rx_bw_BW500KHZ_IF500KHZ == bw_en) return 500000;
    if (at86rf215_radio_rx_bw_BW630KHZ_IF1000KHZ == bw_en) return 630000;
    if (at86rf215_radio_rx_bw_BW800KHZ_IF1000KHZ == bw_en) return 800000;
    if (at86rf215_radio_rx_bw_BW1000KHZ_IF1000KHZ == bw_en) return 1000000;
    if (at86rf215_radio_rx_bw_BW1250KHZ_IF2000KHZ == bw_en) return 1250000;
    if (at86rf215_radio_rx_bw_BW1600KHZ_IF2000KHZ == bw_en) return 1600000;
    if (at86rf215_radio_rx_bw_BW2000KHZ_IF2000KHZ == bw_en) return 2000000;
    return 2000000;
}

//========================================================
static at86rf215_radio_tx_cut_off_en convertTxBandwidth(double bw_numeric)
{
    if (fabs(bw_numeric - 80000) < 1) return at86rf215_radio_tx_cut_off_80khz;
    if (fabs(bw_numeric - 100000) < 1) return at86rf215_radio_tx_cut_off_100khz;
    if (fabs(bw_numeric - 125000) < 1) return at86rf215_radio_tx_cut_off_125khz;
    if (fabs(bw_numeric - 160000) < 1) return at86rf215_radio_tx_cut_off_160khz;
    if (fabs(bw_numeric - 200000) < 1) return at86rf215_radio_tx_cut_off_200khz;
    if (fabs(bw_numeric - 250000) < 1) return at86rf215_radio_tx_cut_off_250khz;
    if (fabs(bw_numeric - 315000) < 1) return at86rf215_radio_tx_cut_off_315khz;
    if (fabs(bw_numeric - 400000) < 1) return at86rf215_radio_tx_cut_off_400khz;
    if (fabs(bw_numeric - 500000) < 1) return at86rf215_radio_tx_cut_off_500khz;
    if (fabs(bw_numeric - 625000) < 1) return at86rf215_radio_tx_cut_off_625khz;
    if (fabs(bw_numeric - 800000) < 1) return at86rf215_radio_tx_cut_off_800khz;
    if (fabs(bw_numeric - 1000000) < 1) return at86rf215_radio_tx_cut_off_1000khz;
    return at86rf215_radio_tx_cut_off_1000khz;
}

//========================================================
static double convertTxBandwidth(at86rf215_radio_tx_cut_off_en bw_en)
{
    if (at86rf215_radio_tx_cut_off_80khz == bw_en) return 80000;
    if (at86rf215_radio_tx_cut_off_100khz == bw_en) return 100000;
    if (at86rf215_radio_tx_cut_off_125khz == bw_en) return 125000;
    if (at86rf215_radio_tx_cut_off_160khz == bw_en) return 160000;
    if (at86rf215_radio_tx_cut_off_200khz == bw_en) return 200000;
    if (at86rf215_radio_tx_cut_off_250khz == bw_en) return 250000;
    if (at86rf215_radio_tx_cut_off_315khz == bw_en) return 315000;
    if (at86rf215_radio_tx_cut_off_400khz == bw_en) return 400000;
    if (at86rf215_radio_tx_cut_off_500khz == bw_en) return 500000;
    if (at86rf215_radio_tx_cut_off_625khz == bw_en) return 625000;
    if (at86rf215_radio_tx_cut_off_800khz == bw_en) return 800000;
    if (at86rf215_radio_tx_cut_off_1000khz == bw_en) return 1000000;
    return 1000000;
}


//========================================================
void Cariboulite::setBandwidth( const int direction, const size_t channel, const double bw )
{
    printf("setBandwidth\n");

    if (direction == SOAPY_SDR_RX)
    {
        cariboulite_set_rx_bandwidth(&radios,(cariboulite_channel_en)channel, 
                        convertRxBandwidth(bw));
    }
    else if (direction == SOAPY_SDR_TX)
    {
        cariboulite_set_tx_bandwidth(&radios,(cariboulite_channel_en)channel, 
                                convertTxBandwidth(bw));
    }
}

//========================================================
double Cariboulite::getBandwidth( const int direction, const size_t channel ) const
{
    printf("getBandwidth\n");
    
    if (direction == SOAPY_SDR_RX)
    {
        at86rf215_radio_rx_bw_en bw;
        cariboulite_get_rx_bandwidth((cariboulite_radios_st*)&radios,(cariboulite_channel_en)channel, &bw);
        return convertRxBandwidth(bw);
    }
    else if (direction == SOAPY_SDR_TX)
    {
        at86rf215_radio_tx_cut_off_en bw;
        cariboulite_get_tx_bandwidth((cariboulite_radios_st*)&radios,(cariboulite_channel_en)channel, &bw);
        return convertTxBandwidth(bw);
    }
    return 0.0;
}

//========================================================
std::vector<double> Cariboulite::listBandwidths( const int direction, const size_t channel ) const
{
    printf("listBandwidths\n");
    std::vector<double> options;
    if (direction == SOAPY_SDR_RX)
    {
        options.push_back( 160000 );
        options.push_back( 200000 );
        options.push_back( 250000 );
        options.push_back( 320000 );
        options.push_back( 400000 );
        options.push_back( 500000 );
        options.push_back( 630000 );
        options.push_back( 800000 );
        options.push_back( 1000000 );
        options.push_back( 1250000 );
        options.push_back( 1600000 );
        options.push_back( 2000000 );
    }
    else
    {
        options.push_back( 80000 );
        options.push_back( 100000 );
        options.push_back( 125000 );
        options.push_back( 160000 );
        options.push_back( 200000 );
        options.push_back( 250000 );
        options.push_back( 315000 );
        options.push_back( 400000 );
        options.push_back( 500000 );
        options.push_back( 625000 );
        options.push_back( 800000 );
        options.push_back( 1000000 );
    }
    return(options);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
void Cariboulite::setFrequency( const int direction, const size_t channel, const std::string &name, 
                                const double frequency, const SoapySDR::Kwargs &args )
{
    printf("setFrequency\n");
    if (name != "RF")
    {
        return;
    }

    cariboulite_set_frequency(&radios, (cariboulite_channel_en)channel, true, (double *)&frequency);
}

//========================================================
double Cariboulite::getFrequency( const int direction, const size_t channel, const std::string &name ) const
{
    printf("getFrequency\n");
    double freq;
    if (name != "RF")
    {
        return 0.0;
    }

    cariboulite_get_frequency((cariboulite_radios_st*)&radios,(cariboulite_channel_en)channel, &freq, NULL, NULL);
    return freq;
}

//========================================================
SoapySDR::ArgInfoList Cariboulite::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    printf("getFrequencyArgsInfo\n");
    SoapySDR::ArgInfoList freqArgs;
	// TODO: frequency arguments
	return freqArgs;
}

//========================================================
std::vector<std::string> Cariboulite::listFrequencies( const int direction, const size_t channel ) const
{
    printf("listFrequencies\n");
    // on both sub1ghz and the wide channel, the RF frequency is controlled
    std::vector<std::string> names;
	names.push_back( "RF" );
	return(names);
}

//========================================================
SoapySDR::RangeList Cariboulite::getFrequencyRange( const int direction, const size_t channel, const std::string &name ) const
{
    printf("getFrequencyRange\n");
    if (name != "RF" )
    {
		throw std::runtime_error( "getFrequencyRange(" + name + ") unknown name" );
    }

    if (channel == cariboulite_channel_s1g)
    {
        SoapySDR::RangeList list;
        list.push_back(SoapySDR::Range( 389.5e6, 510e6 ));
        list.push_back(SoapySDR::Range( 779e6, 1020e6 ));
        return list;
    }
    else if (channel == cariboulite_channel_6g) 
    {
        return (SoapySDR::RangeList( 1, SoapySDR::Range( 50e6, 6000e6 ) ) );
    }
    throw std::runtime_error( "getFrequencyRange - unknown channel" );
}

//========================================================
std::vector<std::string> Cariboulite::listSensors(const int direction, const size_t channel) const
{
    std::vector<std::string> lst;

	if (direction == SOAPY_SDR_RX) lst.push_back( "RSSI" );
    if (direction == SOAPY_SDR_RX) lst.push_back( "ENERGY" );
    lst.push_back( "PLL_LOCK_MODEM" );
    if (channel == cariboulite_channel_6g)
    {
        lst.push_back( "PLL_LOCK_MIXER" );
    }

	return(lst);
}

//========================================================
SoapySDR::ArgInfo Cariboulite::getSensorInfo(const int direction, const size_t channel, const std::string &key) const
{
    SoapySDR::ArgInfo info;
    if (direction == SOAPY_SDR_RX)
    {
        if (key == "RSSI")
        {
            info.name = "RX RSSI";
            info.key = "RSSI";
            info.type = info.FLOAT;
            info.description = "Modem level RSSI measurment";
            info.range = SoapySDR::Range(-127, 4);
            return info;
        }
        if (key == "ENERGY")
        {
            info.name = "RX ENERGY";
            info.key = "ENERGY";
            info.type = info.FLOAT;
            info.description = "Modem level ENERGY (EDC) measurment";
            info.range = SoapySDR::Range(-127, 4);
            return info;
        }
    }

    if (key == "PLL_LOCK_MODEM")
    {
        info.name = "PLL Lock Modem";
        info.key = "PLL_MODEM";
        info.type = info.BOOL;
        info.description = "Modem PLL locking indication";
        return info;
    }

    if (channel == cariboulite_channel_6g && key == "PLL_LOCK_MIXER")
    {
        info.name = "PLL Lock Mixer";
        info.key = "PLL_MIXER";
        info.type = info.BOOL;
        info.description = "Mixer LO PLL locking indication";
        return info;
    }
    return info;
}

//========================================================
std::string Cariboulite::readSensor(const int direction, const size_t channel, const std::string &key) const
{
    return std::to_string(readSensor<float>(direction, channel, key));
}

//========================================================
template <typename Type>
Type Cariboulite::readSensor(const int direction, const size_t channel, const std::string &key) const
{
    if (direction == SOAPY_SDR_RX)
    {
        if (key == "RSSI")
        {
            float rssi = 0.0f;
            cariboulite_get_rssi((cariboulite_radios_st*)&radios,(cariboulite_channel_en)channel, &rssi);
            return rssi;
        }
        if (key == "ENERGY")
        {
            float energy = 0.0f;
            cariboulite_get_energy_det((cariboulite_radios_st*)&radios,(cariboulite_channel_en)channel, &energy);
            return energy;
        }
    }

    cariboulite_radio_state_st* rad = (cariboulite_radio_state_st* )
                                ((channel == cariboulite_channel_s1g) ? &radios.radio_sub1g : &radios.radio_6g);

    if (key == "PLL_LOCK_MODEM")
    {
        return rad->modem_pll_locked;
    }

    if (channel == cariboulite_channel_6g && key == "PLL_LOCK_MIXER")
    {
        return rad->lo_pll_locked;
    }
    return 0;
}