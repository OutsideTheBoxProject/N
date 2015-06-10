boolean SDinit() {

  pinMode(sdCSPin, OUTPUT);
  if (SD.begin(sdCSPin))
    return true;
  return false; 
}

void printTimestamp(File logFile, tmElements_t tm) {

      logFile.print(tmYearToCalendar(tm.Year));
      logFile.print("-");
      logFile.print(tm.Month);
      logFile.print("-");
      logFile.print(tm.Day);
      logFile.print(" ");
      logFile.print(twoDigits(tm.Hour));
      logFile.print(":");
      logFile.print(twoDigits(tm.Minute));
      logFile.print(":");
      logFile.print(twoDigits(tm.Second));
}
  
  
  
  
