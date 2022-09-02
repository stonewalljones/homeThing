#include "esphomeRemote.h"

#ifndef REMOTEFIRE
#define REMOTEFIRE

enum RemoteLightState {
  OffRemoteLightState,
  OnRemoteLightState,
  EffectRemoteLightState
};

RemoteLightState lightState = OffRemoteLightState;

void manageLight() {
  if (!id(backlight).state) {
    if (lightState != OffRemoteLightState) {
      auto call = id(side_light).turn_off();
      call.perform();
      lightState = OffRemoteLightState;
    }
  } else if (lightState == OffRemoteLightState && !speakerGroup -> playerSearchFinished) {
    auto call = id(side_light).turn_on();
    call.set_effect("Rainbow Effect");
    call.perform();
    lightState = EffectRemoteLightState;
  } else if (lightState != OnRemoteLightState && speakerGroup -> playerSearchFinished) {
    auto offCall = id(side_light).turn_off();
    offCall.perform();
    auto call = id(side_light).turn_on();
    call.set_brightness(0.5);
    call.set_rgb(0.5, 0.25, 1.0);
    call.perform();
    lightState = OnRemoteLightState;
  }
}

void selectNowPlayingMenu() {
  if(activeMenuTitleCount <= 0 && menuIndex < activeMenuTitleCount) {
    return;
  }
  auto menuTitle = speakerNowPlayingMenuStates()[menuIndex];
  switch(menuTitle) {
  case pauseNowPlayingMenuState:
    speakerGroup -> activePlayer -> playPause();
    break;
  case volumeUpNowPlayingMenuState:
    speakerGroup -> increaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    break;
  case volumeDownNowPlayingMenuState:
    speakerGroup -> decreaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    break;
  case nextNowPlayingMenuState:
    speakerGroup -> activePlayer -> nextTrack();
    break;
  case shuffleNowPlayingMenuState:
    speakerGroup -> toggleShuffle();
    break;
  case menuNowPlayingMenuState:
    topMenu();
  }
  displayUpdate.updateDisplay(true);
}

void buttonPressSelect() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("power");
      displayUpdate.updateDisplay(true);
      return;
    }

    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup -> tv -> tvRemoteCommand("play");
      break;
    case SpeakerRemotePlayerType:
      selectNowPlayingMenu();
      return;
    }
    return;
  default:
    break;
  }
  if (selectMenu()) {
    displayUpdate.updateDisplay(true);
  }
}

void buttonPressLeft() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else if(activeMenuState == nowPlayingMenu) {
    menuIndex = activeMenuTitleCount - 1;
  } else {
    topMenu();
  }
  displayUpdate.updateDisplay(true);
}

void buttonPressRight() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  } else {
    menuIndex = 0;
  }
  displayUpdate.updateDisplay(true);
}

#endif