#pragma once
// Shim de CrossPointRoots para firmware derivado de Witchhunt.
// En el dispositivo esto es un bundle curado de raices de confianza para el
// TLS de wolfSSL. En escritorio el HTTPS lo resuelve curl del anfitrion con
// el almacen de certificados del sistema, asi que basta con el simbolo vacio
// para que setCACert() tenga algo que recibir.
inline constexpr char CROSSPOINT_ROOTS_PEM[] = "";
