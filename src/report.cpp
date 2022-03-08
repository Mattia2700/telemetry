#include "report.h"

jmp_buf env;

void error_handler  (HPDF_STATUS   error_no,
                HPDF_STATUS   detail_no,
                void         *user_data)
{
    printf ("ERROR: error_no=%04X, detail_no=%u   %04X\n", (HPDF_UINT)error_no,
                (HPDF_UINT)detail_no, (HPDF_UINT)detail_no);
    longjmp(env, 1);
}

void
show_description (HPDF_Page    page,
                  float        x,
                  float        y,
                  const char  *text)
{
    char buf[255];

    HPDF_Page_MoveTo (page, x, y - 10);
    HPDF_Page_LineTo (page, x, y + 10);
    HPDF_Page_MoveTo (page, x - 10, y);
    HPDF_Page_LineTo (page, x + 10, y);
    HPDF_Page_Stroke (page);

    HPDF_Page_SetFontAndSize (page, HPDF_Page_GetCurrentFont (page), 8);
    HPDF_Page_SetRGBFill (page, 0, 0, 0);

    HPDF_Page_BeginText (page);

#ifdef __WIN32__
    _snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#else
    snprintf(buf, 255, "(x=%d,y=%d)", (int)x, (int)y);
#endif /* __WIN32__ */
    HPDF_Page_MoveTextPos (page, x - HPDF_Page_TextWidth (page, buf) - 5,
            y - 10);
    HPDF_Page_ShowText (page, buf);
    HPDF_Page_EndText (page);

    HPDF_Page_BeginText (page);
    HPDF_Page_MoveTextPos (page, x - 20, y - 25);
    HPDF_Page_ShowText (page, text);
    HPDF_Page_EndText (page);
}

void Report::AddDeviceSample(Chimera* chim, Device* device)
{
  if (device == chim->accel){
    Imu* accel = (Imu*) device;
    sensor_data["accel"]["timestamp"].push_back(accel->timestamp);
    sensor_data["accel"]["x"].push_back(accel->x);
    sensor_data["accel"]["y"].push_back(accel->y);
    sensor_data["accel"]["z"].push_back(accel->z);
  }else if(device == chim->gyro){
    Imu* gyro = (Imu*) device;
    sensor_data["gyro"]["timestamp"].push_back(gyro->timestamp);
    sensor_data["gyro"]["x"].push_back(gyro->x);
    sensor_data["gyro"]["y"].push_back(gyro->y);
    sensor_data["gyro"]["z"].push_back(gyro->z);
  }else if(device == chim->encoder_left){
    Encoder* enc = (Encoder*) device;
    sensor_data["encoder_l"]["timestamp"].push_back(enc->timestamp);
    sensor_data["encoder_l"]["rads"].push_back(enc->rads);
    sensor_data["encoder_l"]["km"].push_back(enc->km);
    sensor_data["encoder_l"]["rotations"].push_back(enc->rotations);
  }else if(device == chim->encoder_right){
    Encoder* enc = (Encoder*) device;
    sensor_data["encoder_r"]["timestamp"].push_back(enc->timestamp);
    sensor_data["encoder_r"]["rads"].push_back(enc->rads);
    sensor_data["encoder_r"]["km"].push_back(enc->km);
    sensor_data["encoder_r"]["rotations"].push_back(enc->rotations);
  }else if(device == chim->inverter_right){
    Inverter* inv = (Inverter*) device;
    sensor_data["inverter_r"]["timestamp"].push_back(inv->timestamp);
    sensor_data["inverter_r"]["temperature"].push_back(inv->temperature);
    sensor_data["inverter_r"]["motor_temp"].push_back(inv->motor_temp);
    sensor_data["inverter_r"]["torque"].push_back(inv->torque);
    sensor_data["inverter_r"]["speed"].push_back(inv->speed);
  }else if(device == chim->inverter_left){
    Inverter* inv = (Inverter*) device;
    sensor_data["inverter_l"]["timestamp"].push_back(inv->timestamp);
    sensor_data["inverter_l"]["temperature"].push_back(inv->temperature);
    sensor_data["inverter_l"]["motor_temp"].push_back(inv->motor_temp);
    sensor_data["inverter_l"]["torque"].push_back(inv->torque);
    sensor_data["inverter_l"]["speed"].push_back(inv->speed);
  }else if(device == chim->bms_lv){
    Bms* bms = (Bms*)device;
    sensor_data["bms_lv"]["timestamp"].push_back(bms->timestamp);
    sensor_data["bms_lv"]["temperature"].push_back(bms->temperature);
    sensor_data["bms_lv"]["max_temperature"].push_back(bms->max_temperature);
    sensor_data["bms_lv"]["voltage"].push_back(bms->voltage);
  }else if(device == chim->bms_hv){
    Bms* bms = (Bms*)device;
    sensor_data["bms_hv"]["timestamp"].push_back(bms->timestamp);
    sensor_data["bms_hv"]["temperature"].push_back(bms->temperature);
    sensor_data["bms_hv"]["max_temperature"].push_back(bms->max_temperature);
    sensor_data["bms_hv"]["min_temperature"].push_back(bms->min_temperature);
    sensor_data["bms_hv"]["current"].push_back(bms->current);
    sensor_data["bms_hv"]["voltage"].push_back(bms->voltage);
    sensor_data["bms_hv"]["max_voltage"].push_back(bms->max_voltage);
    sensor_data["bms_hv"]["min_voltage"].push_back(bms->min_voltage);
    sensor_data["bms_hv"]["power"].push_back(bms->power);
  }else if(device == chim->pedal){
    Pedals* pedals = (Pedals*)device;
    sensor_data["pedals"]["timestamp"].push_back(pedals->timestamp);
    sensor_data["pedals"]["throttle1"].push_back(pedals->throttle1);
    sensor_data["pedals"]["throttle2"].push_back(pedals->throttle2);
    sensor_data["pedals"]["brake_front"].push_back(pedals->brake_front);
    sensor_data["pedals"]["brake_rear"].push_back(pedals->brake_rear);
  }else if(device == chim->steer){
    Steer* steer = (Steer*)device;
    sensor_data["steer"]["timestamp"].push_back(steer->timestamp);
    sensor_data["steer"]["angle"].push_back(steer->angle);
  }else if(device == chim->ecu_state){
    
  }else if(device == chim->bms_hv_state){
    
  }else if(device == chim->steering_wheel_state){
    
  }else if(device == chim->ecu){
    
  }else if(device == chim->gps1){
    
  }else if(device == chim->gps2){
    
  }
}

void Report::Filter(const vector<double> &in, vector<double>* out, int window)
{
  double mean;
  out->reserve(in.size()-window);
  for(int i = 0; i < in.size()-window; i++)
  {
    mean = 0;
    for(int j = 0; j < window; j++)
      mean += in[i+j];
    mean /= window;
    out->push_back(mean);
  }
}

void Report::Clean(int count)
{
  for(const auto& element : sensor_data)
  {
    for(const auto& subelement : sensor_data[element.first])
    {
      if(sensor_data[element.first][subelement.first].size() > count)
      {
        int factor = sensor_data[element.first][subelement.first].size() / count;
        vector<double> out;
        out.reserve(count);
        for(int k = 0; k < count; k++)
        {
          out.push_back(sensor_data[element.first][subelement.first][k*factor]);
        }
        sensor_data[element.first][subelement.first].clear();
        Filter(out, &sensor_data[element.first][subelement.first]);
      }

      cout << sensor_data[element.first][subelement.first].size() << endl;
    }
  }
}


void Report::Generate(string path)
{
  HPDF_Doc  pdf;
  HPDF_Font font;
  HPDF_Page page;
  string fname = "test.pdf";
  HPDF_Destination dst;
  HPDF_Image image;


  vector<vector<MapElement>> coupled_graphs {
    {
      {"accel", "timestamp"},
      {"accel", "x"},
      {"accel", "y"},
      {"accel", "z"}
    },
    {
      {"gyro", "timestamp"},
      {"gyro", "x"},
      {"gyro", "y"},
      {"gyro", "z"}
    },
    {
      {"encoder_l", "timestamp"},
      {"encoder_l", "rads"},
      {"encoder_r", "rads"}
    },
    {
      {"inverter_l", "timestamp"},
      {"inverter_l", "speed"},
      {"inverter_r", "speed"}
    },
    {
      {"encoder_l", "timestamp"},
      {"encoder_l", "rads"},
      {"encoder_r", "rads"},
      {"inverter_l", "speed"},
      {"inverter_r", "speed"}
    },
    {
      {"pedals", "timestamp"},
      {"pedals", "throttle1"},
      {"pedals", "brake_front"},
      {"pedals", "brake_rear"},
    },
    {
      {"encoder_l", "timestamp"},
      {"encoder_l", "rads"},
      {"pedals", "throttle1"},
      {"pedals", "brake_front"},
    },
    {
      {"gyro", "timestamp"},
      {"gyro", "z"},
      {"steer", "angle"}
    },
    {
      {"accel", "timestamp"},
      {"accel", "x"},
      {"pedals", "throttle1"},
      {"pedals", "brake_front"}
    }
  };

  double x;
  double y;
  double angle;
  double angle1;
  double angle2;
  double rad;
  double rad1;
  double rad2;

  double iw;
  double ih;

  pdf = HPDF_New (error_handler, NULL);

  if (!pdf) {
      cout << "error: cannot create PdfDoc object" << endl;
      return;
  }


  /* error-handler */
  if (setjmp(env)) {
      HPDF_Free (pdf);
      return;
  }

  HPDF_SetCompressionMode (pdf, HPDF_COMP_NONE);

  /* create default-font */
  font = HPDF_GetFont (pdf, "Helvetica", NULL);

  {
    page = HPDF_AddPage (pdf);

    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
    HPDF_SetOpenAction(pdf, dst);

    HPDF_Page_BeginText (page);
    HPDF_Page_SetFontAndSize (page, font, 24);
    HPDF_Page_MoveTextPos (page, 20, HPDF_Page_GetHeight (page) - 70);
    HPDF_Page_ShowText (page, "Eagle TRT Telemetry Report");
    HPDF_Page_EndText (page);
  }


  for(int i = 0; i < coupled_graphs.size(); i++)
  {
    const std::vector<MapElement>& graph = coupled_graphs[i];

    HPDF_Page page = HPDF_AddPage (pdf);

    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);

    dst = HPDF_Page_CreateDestination (page);
    HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);

    {
      //Gnuplot gp;
      Gnuplot gp("tee plot.gnu | gnuplot -persist");

      gp << "set terminal png size 1920,1080\n";
      gp << "set output 'graph_" << i << ".png'\n";
      gp << "set xrange ["<< int(sensor_data[graph[0].primary][graph[0].secondary][0]);
      gp << ":" << int(sensor_data[graph[0].primary][graph[0].secondary].back()) << "]\n";
      // gp << "set yrange [-8:8]\n";

      stringstream command;
      command << "plot ";
      for(int j = 1; j < graph.size(); j++)
      {
        string title = graph[j].primary + " " + graph[j].secondary;
        int idx = title.find('_');
        if(idx != string::npos)
          title[idx] = '-';
        command << "'-' with lines title '" << title << "'";
        if(j == graph.size()-1)
          command << "\n";
        else
          command << ", ";
      }

      cout << command.str() << endl;
      gp << command.str();

      for(int j = 1; j < graph.size(); j++)
      {
        cout << graph[0].primary << " " << graph[0].secondary << "\t";
        cout << graph[j].primary << " " << graph[j].secondary << endl;

        gp.send1d(make_pair(
          sensor_data[graph[0].primary][graph[0].secondary],
          sensor_data[graph[j].primary][graph[j].secondary]
          ));
      }
    }

    


    /* load image file. */
    image = HPDF_LoadPngImageFromFile (pdf, string("graph_"+to_string(i)+".png").c_str());

    iw = HPDF_Image_GetWidth (image);
    ih = HPDF_Image_GetHeight (image);
    float ratio = iw / ih;
    iw = HPDF_Page_GetWidth(page);
    ih = iw/ratio;

    x = (HPDF_Page_GetWidth(page) - iw)/2;
    if(x < 0)
      x = 0;
    y = HPDF_Page_GetHeight(page) - ih;

    /* Draw image to the canvas. (normal-mode with actual size.)*/
    HPDF_Page_DrawImage (page, image, x, y, iw, ih);

    // fs::remove("graph.png");
  }


  /* save the document to a file */
  HPDF_SaveToFile (pdf, fname.c_str());

  /* clean up */
  HPDF_Free (pdf);
}