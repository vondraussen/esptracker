// https://simcom.ee/documents/SIM7000x/SIM7000%20Series_GNSS_Application%20Note_V1.01.pdf
// http://xtrapath1.izatcloud.net/xtra3grc.bin
// download file in psram
// upload to modem via AT commands to /customer/xtra3grc.bin (AT+CFSWFILE)
// Copy XTRA file
// Open XTRA function
// Cold start GNSS
// check for Aid XTRA file success

    //case AGPS_SETUP:
    // file test
    // agpsFile = SPIFFS.open("/xtra3grc.bin");
    // if (!agpsFile) {
    //   DBG("Failed to open agpsFile for reading");
    //   return;
    // }
    // agpsFileSize = agpsFile.size();
    // DBG("agpsFile size: ", agpsFileSize);

    // copy agps file from spiffs to modem fs
    // AT+CFSWFILE
    // modem.sendAT("+CFSINIT");
    // modem.waitResponse();
    // // 1
    // sprintf(strBuf, "+CFSWFILE=3,\"xtra3grc.bin\",0,%d,10000", 10000);
    // DBG("## ", strBuf);
    // modem.sendAT(strBuf);
    // modem.waitResponse();
    // agpsFile.read(fileBuf, 10000);
    // modem.stream.write(fileBuf, 10000);
    // modem.stream.flush();
    // modem.waitResponse();
    // // 2
    // sprintf(strBuf, "+CFSWFILE=3,\"xtra3grc.bin\",1,%d,10000", 10000);
    // DBG("## ", strBuf);
    // modem.sendAT(strBuf);
    // modem.waitResponse();
    // agpsFile.read(fileBuf, 10000);
    // modem.stream.write(fileBuf, 10000);
    // modem.stream.flush();
    // modem.waitResponse();
    // // 2
    // sprintf(strBuf, "+CFSWFILE=3,\"xtra3grc.bin\",1,%d,10000", agpsFileSize - 20000);
    // DBG("## ", strBuf);
    // modem.sendAT(strBuf);
    // modem.waitResponse();
    // agpsFile.read(fileBuf, agpsFileSize - 20000);
    // modem.stream.write(fileBuf, agpsFileSize - 20000);
    // modem.stream.flush();
    // modem.waitResponse();

    // agpsFile.close();
    // modem.sendAT("+CFSTERM");
    // modem.waitResponse();
    // modem.sendAT("+CGNSMOD=1,1,0,2");
    // modem.waitResponse();
    // modem.sendAT("+CNTPCID=1");
    // modem.waitResponse();
    // modem.sendAT("+CNTP=\"ntp1.aliyun.com\",4,1");
    // modem.waitResponse();
    // modem.sendAT("+CNTP");
    // modem.waitResponse();
    // modem.sendAT("+CNACT=1,\"pinternet.interkom.de\"");
    // modem.waitResponse();
    // modem.sendAT(
    //     "+HTTPTOFS=\"http://xtrapath1.izatcloud.net/xtra3grc.bin\",\"/"
    //     "customer/xtra3grc.bin\"");
    // modem.waitResponse(50000L);
    // modem.sendAT("+CNACT=0");
    // modem.waitResponse();

    // copy file from modem fs to GPS
    // modem.sendAT("+CGNSCPY");
    // modem.waitResponse(10000L, "\r\n" "+CGNSCPY:");
    // res = modem.stream.readStringUntil('\n');
    // modem.waitResponse();
    // res.trim();
    // DBG("+CGNSCPY:", res); // should be 0

    // modem.sendAT("+CGNSXTRA=1");
    // modem.waitResponse();
    // modem.sendAT("+CGNSHOT");  // check response +CGNSXTRA: <mod>
    // // modem.sendAT("+CGNSCOLD");  // check response +CGNSXTRA: <mod>
    // modem.waitResponse();
    // modem.waitResponse(
    //     "\r\n"
    //     "+CGNSXTRA:");
    // res = modem.stream.readStringUntil('\n');
    // DBG("modem coldstart ", res);  // needs to be a 0 for success
    // state = GPRS_CONNECTED;
    // break;