#pragma once

void webServerInit();   // register routes and start listening on port 80
void webServerHandle();  // call from loop()