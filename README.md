# POS API Demo Project

## Overview

This project demonstrates how to integrate API GET and POST requests into a POS application, following the provided documentation and demo structure.

## Features

- **API Test Menu:**  
  Press the FUNCTION key on the POS device to open the API Test menu.
- **Menu Options:**  
  - Press `1` for API GET Test (displays the "title" from the sample API).
  - Press `2` for API POST Test (displays HTTP response code and body).
  - Press CANCEL to exit the menu.
- **Results** are displayed on the POS device screen using POS display functions.

## How to Use

1. Build and run the POS app.
2. On the main screen, press the FUNCTION key.
3. In the API Test menu:
    - Press `1` for GET request.
    - Press `2` for POST request.
    - Press CANCEL to exit.

## Collaboration

- The project is on GitHub: [https://github.com/Raviteja447/pos-api-demo](https://github.com/Raviteja447/pos-api-demo)
- Collaborator: `antiergit` has been added.

## Files

- `src/main.c`: Main application loop and menu integration.
- `src/ApiTest.c`: API GET/POST logic and POS display.
- `inc/ApiTest.h`: API test function declarations.
- Other `src/` and `inc/` files: Standard POS project files.

---

