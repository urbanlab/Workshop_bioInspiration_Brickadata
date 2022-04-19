class Slave
{
private:

  // tableau de ports d'entrees analogiques
  int _ins[N_ANALOG_INPUTS];
  // seuils de detection des cubes
  float _reference_values[N_ANALOG_INPUTS];

  // index de l'arduino dans la chaine, definit par SLAVE_INDEX
  int _slave_index;
  // taille des donnees locales, nombre de positions *2 (deux circuits par position)
  int _size_data;
  // taille des donnees recues, 3 => [position,nb type 1, nb type 2]
  int _size_read;
  // taille des donnees transmises, 3 => [position,nb type 1, nb type 2]
  int _size_write;
  // donnees locales, pour les N_IN position gerees par un arduino (*2 car deux circuits par position)
  DynamicJsonDocument * _data;
  // donnees recus
  DynamicJsonDocument * _data_read;
  // donnees transmises
  DynamicJsonDocument * _data_write;
  // canal de communication
  SoftwareSerial* _serial;

  // position courante transmise pour assurer la synchro 
  int _current_pos;

public:

  void setup(int slave_index);
  void loop();
  void readData();
  void transferData();
  void sendData(int pos);
  ~Slave();
};

void Slave::setup(int slave_index)
{
  _slave_index = slave_index;
  _size_data = N_IN *2;
  _size_read = 3;
  _size_write = 3;

  _current_pos = 0;

  _data = new DynamicJsonDocument(JSON_OBJECT_SIZE(_size_data));
  _data_read = new DynamicJsonDocument(JSON_OBJECT_SIZE(_size_read));
  _data_write = new DynamicJsonDocument(JSON_OBJECT_SIZE(_size_write));

  // lecture sur le port 10, ecriture sur le port 11
  _serial = new SoftwareSerial(10,11);
  _serial->begin(4800);

  // initialisation des donnees locales
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

void Slave::loop()
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
      (*_data)[i*2] = cubes1;
      (*_data)[i*2+1] = cubes2;
      sendData(i);
      _current_pos = i;
    }
  }

  // lit en boucle meme si pas de changement pour eviter les messages qui se seraient perdus
  // lit la position courante
  {
    // lecture du couples d'entrees analogiques pour la position courante
    int v1 = analogRead(_ins[_current_pos*2]);
    int v2 = analogRead(_ins[_current_pos*2+1]);

    int cubes1 = round(v1/_reference_values[_current_pos*2]);
    int cubes2 = round(v2/_reference_values[_current_pos*2+1]);

    // Serial.println(cubes1);
    // Serial.println(cubes2);

    (*_data)[_current_pos*2] = cubes1;
    (*_data)[_current_pos*2+1] = cubes2;
    sendData(_current_pos);

    _current_pos++;
    _current_pos %= N_IN;
  }

  // le premier arduino n'attend rien en lecture
  if ( _slave_index == 0 ){}
  // les autres arduino lisent les donnees entrantes
  else { this->readData(); }
}

void Slave::sendData(int pos)
{
  // la position globale = index de l'arduino * nombre de position par arduino + position locale
  (*_data_write)[0] = (_slave_index*N_IN)+pos;
  // nombre de cubes de type 1
  (*_data_write)[1] = (*_data)[pos*2];
  // nombre de cubes de type 2
  (*_data_write)[2] = (*_data)[pos*2+1];

  serializeJson(*_data_write, *_serial);

  
  Serial.println("data changed");
  Serial.print((int)(*_data_write)[0]);
  Serial.print(",");
  Serial.print((int)(*_data_write)[1]);
  Serial.print(",");
  Serial.print((int)(*_data_write)[2]);
  Serial.println(";");
  
}

void Slave::transferData()
{
  // transmet les donnees recus au niveau inferieur
  (*_data_write)[0] = (*_data_read)[0];
  (*_data_write)[1] = (*_data_read)[1];
  (*_data_write)[2] = (*_data_read)[2];
  
  serializeJson(*_data_write, *_serial);

  
  Serial.println("transfer data");
  Serial.print((int)(*_data_write)[0]);
  Serial.print(",");
  Serial.print((int)(*_data_write)[1]);
  Serial.print(",");
  Serial.print((int)(*_data_write)[2]);
  Serial.println(";");
  
}

void Slave::readData()
{
  if ((*_serial).available()) 
  {
    DynamicJsonDocument data_received(JSON_OBJECT_SIZE(_size_read) + 20);
    DeserializationError err = deserializeJson(data_received, (*_serial));

    if (err == DeserializationError::Ok) 
    {
      // si l'on recoit une donnee on la transfere au niveau inferieur
      for (int i = 0; i < _size_read; i++ )
      {
        (*_data_read)[i] = data_received[i].as<int>();
        // Serial.print((int)(*_data_read)[i]);
        // Serial.print(",");
      }
      // Serial.println(";");

      this->transferData();
    } 
    else 
    {
      // Serial.print("deserialize error on slave ");
      // Serial.print(_slave_index);
      // Serial.print(" - ");
      // Serial.println(err.c_str());
      // Flush all bytes in the "link" serial port buffer
      while ((*_serial).available() > 0) { (*_serial).read(); }
    }
  }
}

Slave::~Slave()
{
  delete _data;
  delete _data_read;
  delete _data_write;
  delete _serial;
}
