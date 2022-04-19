class Master
{
private:

  // tableau de ports d'entrees analogiques
  int _ins[N_ANALOG_INPUTS];
  // seuils de detection des cubes
  float _reference_values[N_ANALOG_INPUTS];

  // taille des donnees recues, 3 => [position,nb type 1, nb type 2]
  int _size_read;
  // taille des donnees globale, nombre de positions *2 (deux circuits par position)
  int _size_data;
  // donnees recus
  DynamicJsonDocument * _data_read;
   // donnees globales
  DynamicJsonDocument * _data;
  // canal de communication
  SoftwareSerial* _serial;

  // position courante transmise pour assurer la synchro 
  int _current_pos;

public:

  void setup();
  void loop();
  void readData();
  void concatData();
  void dataChanged(int pos, int v1, int v2);
  void printData();

  ~Master();
};

void Master::setup()
{
  _size_read = 3;
  _size_data = N_ARDUINO*N_IN*2;

  _current_pos = 0;

  _data_read = new DynamicJsonDocument(JSON_OBJECT_SIZE(_size_read));
  _data = new DynamicJsonDocument(JSON_OBJECT_SIZE(_size_data));

  // lecture sur le port 10, ecriture sur le port 11 (MASTER n'ecrit rien)
  _serial = new SoftwareSerial(10,11);
  _serial->begin(4800);

  // initialisation des donnees globales
  for (int i=0; i < _size_data; i++) { (*_data)[i] = 0; }

  if (N_ANALOG_INPUTS > 0) { _ins[0] = A0; }
  if (N_ANALOG_INPUTS > 1) { _ins[1] = A1; }
  if (N_ANALOG_INPUTS > 2) { _ins[2] = A2; }
  if (N_ANALOG_INPUTS > 3) { _ins[3] = A3; }
  if (N_ANALOG_INPUTS > 4) { _ins[4] = A4; }
  if (N_ANALOG_INPUTS > 5) { _ins[5] = A5; }
  if (N_ANALOG_INPUTS > 6) { _ins[6] = A6; }
  if (N_ANALOG_INPUTS > 7) { _ins[7] = A7; }
  if (N_ANALOG_INPUTS > 8) { _ins[8] = A8; }
  if (N_ANALOG_INPUTS > 9) { _ins[9] = A9; }
  if (N_ANALOG_INPUTS > 10) { _ins[10] = A10; }
  if (N_ANALOG_INPUTS > 11) { _ins[11] = A11; }
  if (N_ANALOG_INPUTS > 12) { _ins[12] = A12; }
  if (N_ANALOG_INPUTS > 13) { _ins[13] = A13; }

  for (int i = 0; i < N_ANALOG_INPUTS; i++)
  {
    _reference_values[i] = 17;
  }

  // _reference_values[0] = 17;
  // _reference_values[1] = 17;
  // _reference_values[2] = 17;
  // _reference_values[3] = 17;
  // _reference_values[4] = 17;
  // _reference_values[5] = 17;
  // _reference_values[6] = 17;
  // _reference_values[7] = 17;
  // _reference_values[8] = 17;
  // _reference_values[9] = 17;
  // _reference_values[10] = 17;
  // _reference_values[11] = 17;
  // _reference_values[12] = 17;
  // _reference_values[13] = 17;
}

void Master::loop()
{

  // regarde si changement et l'envoi
  for (int i = 0; i < N_IN; i++ )
  {
    int v1 = analogRead(_ins[i*2]);
    int v2 = analogRead(_ins[i*2+1]);

    int cubes1 = round(v1/_reference_values[i*2]);
    int cubes2 = round(v2/_reference_values[i*2+1]);

    if ( cubes1 != (*_data)[i*2] || cubes2 != (*_data)[i*2+1] )
    {
      int pos = i;
      pos += (N_SLAVES*N_IN);
      dataChanged(pos, cubes1, cubes2);
      _current_pos = i;
    }
  }

  // lit la position courante
  {
    // lecture du couples d'entrees analogiques pour la position courante
    int v1 = analogRead(_ins[_current_pos*2]);
    int v2 = analogRead(_ins[_current_pos*2+1]);

    int cubes1 = round(v1/_reference_values[_current_pos*2]);
    int cubes2 = round(v2/_reference_values[_current_pos*2+1]);

    int pos = _current_pos;
    pos += (N_SLAVES*N_IN);

    dataChanged(pos, cubes1, cubes2);

    _current_pos++;
    _current_pos %= N_IN;
  }

  this->readData();
  this->printData();
}

void Master::dataChanged(int pos, int v1, int v2)
{
  (*_data)[pos*2] = v1;
  (*_data)[pos*2+1] = v2;
}

void Master::readData()
{ 
  if ((*_serial).available()) 
  {
    DynamicJsonDocument data_received(JSON_OBJECT_SIZE(_size_read) + 20);
    DeserializationError err = deserializeJson(data_received, (*_serial));

    if (err == DeserializationError::Ok) 
    {
      for (int i = 0; i < _size_read; i++ )
      {
        (*_data_read)[i] = data_received[i].as<int>();
        // Serial.print((int)(*_data_read)[i]);
        // Serial.print(",");
      }
      // Serial.println(";");

      this->concatData();
    } 
    else 
    {
      // Serial.print("deserialize error on master ");
      // Serial.println(err.c_str());
      // Flush all bytes in the "link" serial port buffer
      while ((*_serial).available() > 0) { (*_serial).read(); }
    }
  }
}

void Master::concatData()
{  
  for (int i=0; i < _size_read; i++)
  {
    (*_data)[(int)(*_data_read)[0]*2] = (*_data_read)[1];
    (*_data)[(int)(*_data_read)[0]*2+1] = (*_data_read)[2];
  }
}

void Master::printData()
{ 
  // On ecrit la tableau global sur le serial principal > Processing
  String str = "[";
  for ( int i = 0; i < _size_data; i++ )
  {
    str += (*_data)[i].as<String>();
    str += ",";
  }
  str.setCharAt(str.length()-1, ']');
  Serial.println(str);
}

Master::~Master()
{
  delete _data_read;
  delete _data;
  delete _serial;
}
