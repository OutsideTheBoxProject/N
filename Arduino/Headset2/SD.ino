boolean SDinit(boolean wipe) {

  pinMode(sdCSPin, OUTPUT);
  if (SD.begin(sdCSPin)) {
    if (wipe) {
      File root = SD.open("/");
      File nextFile = root.openNextFile();
      while (nextFile) {
        if (!nextFile.isDirectory())
          SD.remove(nextFile.name());
        nextFile = root.openNextFile();
      }
      root.close(); 
    }
    return true;
  }
  return false; 
}
  
  
  
  
