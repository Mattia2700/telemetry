#include "log_trimmer.h"





int main()
{

  // User select a folder
  Browse b;
  b.SetMaxSelections(1);
  b.SetExtension("*");
  b.SetSelectionType(SelectionType::sel_folder);
  auto selected_paths = b.Start();

  if (selected_paths.size() <= 0)
  {
    cout << "No file selected... exiting" << endl;
    return -1;
  }

  cout << "Selected paths: " << endl;
  for (auto sel : selected_paths)
    cout << "\t" << get_colored(sel, 9, 1) << endl;
  cout << endl;

  // Start the timer
  time_point t_start = high_resolution_clock::now();
  for (string path : selected_paths)
  {
    // Get all the files with that extension
    auto files = get_all_files(path, ".log");

    // Select only candump files
    auto candump_files = get_candump_from_files(files);
    if (candump_files.size() <= 0)
    {
      cout << "No candump found... exiting" << endl;
      return -1;
    }

    for (auto file : candump_files)
      parse_file(file);
  }
  return 0;
}

void parse_file(string fname)
{

  string folder;
  // If the filename has an incremental name create a folder with that number
  // Otherwise create parsed folder
  try
  {
    int n = stoi(remove_extension(fname));
    folder = get_parent_dir(fname) + "/" + to_string(n);
  }
  catch (std::exception &e)
  {
    folder = get_parent_dir(fname) + "/parsed";
  }
  create_directory(folder);

  Chimera chimera;

  // Get all lines
  message msg;
  message msg_back;
  vector<string> lines;
  get_lines(fname, &lines);

  // Contains devices modified from the CAN message
  vector<Device *> modifiedDevices;

  // Start Timer
  time_point t_start = high_resolution_clock::now();

  float threshold = 10.0;
  double tmstmp_threshold = 2.0;

  double start_tmstmp = -0.1;
  double end_tmstmp = -1.0;
  double enc_timestamp = -1.0;
  double pedal_timestamp = -1.0;

  double speed = 0.0;

  uint32_t start_index = 0;

  uint32_t i = 20;

  vector<uint32_t> start_indexes;
  vector<uint32_t> end_indexes;

  Found found = FOUND_NONE;
  while(i >= 20 && i < lines.size())
  {
    switch(found)
    {
      case FOUND_NONE:
      i++;
      break;
      case FOUND_POSSIBLE_START:
      i--;
      break;
      case FOUND_START:
      i++;
      break;
      case FOUND_POSSIBLE_END:
      i++;
      break;
      case FOUND_END:
      break;
    }

    // Try parsing the line
    if (!parse_message(lines[i], &msg))
      continue;
    // Fill the devices
    modifiedDevices = chimera.parse_message(msg.timestamp, msg.id, msg.data, msg.size);

    // For each device modified write the values in the csv file
    for (auto modified : modifiedDevices)
    {
      ////////////////////////////////////////////////////////////////////////////////
      Encoder* enc = dynamic_cast<Encoder*>(modified);
      if(enc != nullptr)
      {
        switch(found)
        {
          case FOUND_NONE:
            speed = max(chimera.encoder_left->rads, chimera.encoder_right->rads);
            if(speed > threshold)
            {
              enc_timestamp = msg.timestamp;
            }
          break;
          case FOUND_POSSIBLE_START:
            speed = max(chimera.encoder_left->rads, chimera.encoder_right->rads);
            if(speed < threshold / 25.0)
            {
              if(start_tmstmp - msg.timestamp > tmstmp_threshold)
              {
                enc_timestamp = msg.timestamp;
              }
            }
          break;
          case FOUND_START:
            speed = max(chimera.encoder_left->rads, chimera.encoder_right->rads);
            if(speed < threshold / 25.0)
            {
              enc_timestamp = msg.timestamp;
            }
          break;
          case FOUND_POSSIBLE_END:
            speed = max(chimera.encoder_left->rads, chimera.encoder_right->rads);
            if(speed < threshold / 25.0)
            {
              if(msg.timestamp - end_tmstmp > tmstmp_threshold)
              {
                enc_timestamp = msg.timestamp;
              }
            }
          break;
        }
      }

      ////////////////////////////////////////////////////////////////////////////////
      Pedals* pedal = dynamic_cast<Pedals*>(modified);
      if(pedal != nullptr)
      {
        switch(found)
        {
          case FOUND_NONE:
          if(pedal->throttle1 > threshold)
          {
            pedal_timestamp = msg.timestamp;
          }
          break;
          case FOUND_POSSIBLE_START:
            if(pedal->throttle1 < threshold)
            {
              if(start_tmstmp - msg.timestamp > tmstmp_threshold)
              {
                pedal_timestamp = msg.timestamp;
              }
            }
          break;
          case FOUND_START:
            if(pedal->throttle1 < threshold)
            {
              pedal_timestamp = msg.timestamp;
            }
          break;
          case FOUND_POSSIBLE_END:
            if(pedal->throttle1 < threshold / 8)
            {
              if(msg.timestamp - end_tmstmp > tmstmp_threshold)
              {
                pedal_timestamp = msg.timestamp;
              }
            }
          break;
        }
      }

      ////////////////////////////////////////////////////////////////////////////////
      if(enc_timestamp > 0 && pedal_timestamp > 0)
      {
        switch(found)
        {
        case FOUND_NONE:
          found = FOUND_POSSIBLE_START;

          start_index = i;

          start_tmstmp = msg.timestamp;
          cout << "Found possible " << fixed << start_tmstmp << endl;

          enc_timestamp = -1.0;
          pedal_timestamp = -1.0;
        break;
        case FOUND_POSSIBLE_START:
          found = FOUND_START;
          start_indexes.push_back(i);
          enc_timestamp = -1.0;
          pedal_timestamp = -1.0;
          cout << "Found start: \n\t" << i << "\n\t" << fixed << msg.timestamp << endl;
          i = start_index;
        break;
        case FOUND_START:
          found = FOUND_POSSIBLE_END;
          enc_timestamp = -1.0;
          pedal_timestamp = -1.0;
          end_tmstmp = msg.timestamp;
        break;
        case FOUND_POSSIBLE_END:
          found = FOUND_END;
          cout << "Found end: \n\t" << i << "\n\t" << fixed << msg.timestamp << endl;
        break;
        }
        break;
      }
    }
  }
}
