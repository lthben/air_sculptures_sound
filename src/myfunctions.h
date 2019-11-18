/*--------------------------------------------------------------------------------
  Read buttons
--------------------------------------------------------------------------------*/
void read_buttons()
{
  if (strip1playMode == IDLE_MODE)
  {
    button0.update();

    if (button0.fallingEdge())
    {
      isButton0Pressed = true;
      Serial.println("button0 pressed");
    }
  }
  if (strip2playMode == IDLE_MODE)
  {
    button1.update();

    if (button1.fallingEdge())
    {
      isButton1Pressed = true;
      Serial.println("button1 pressed");
    }
  }
}

/*--------------------------------------------------------------------------------
  Set playmode
--------------------------------------------------------------------------------*/
void set_playmode() 
{
 
  if (isButton0Pressed == true && strip1playMode == IDLE_MODE) 
  {
    strip1playMode = BUTTON_MODE;
    strip1hasPlayModeChanged = true;
    strip1msec = 0;
  }
  else if (isButton1Pressed == true && strip2playMode == IDLE_MODE) 
  {
    strip2playMode = BUTTON_MODE;
    strip2hasPlayModeChanged = true;
    strip2msec = 0;
  }
  if (strip1playMode == BUTTON_MODE && strip1msec > strip1playDur) //led playback has finished
  {
    strip1playMode = IDLE_MODE;
    isButton0Pressed = false; //start listening again
    strip1hasPlayModeChanged = true;
    Serial.println("strip 1 finished playing\n");
  }
  if (strip2playMode == BUTTON_MODE && strip2msec > strip2playDur)
  {
    strip2playMode = IDLE_MODE;
    isButton1Pressed = false; 
    strip2hasPlayModeChanged = true;
    Serial.println("strip 2 finished playing\n");
  }
}

/*--------------------------------------------------------------------------------
  Play audio
--------------------------------------------------------------------------------*/
void play_audio()
{
  if (strip1playMode == IDLE_MODE && strip2playMode == IDLE_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged == true) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
    }
  }
  else if (strip1playMode == BUTTON_MODE || strip2playMode == BUTTON_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged == true) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
  }
}
