/*
 Copyright 2012 Stéphane Bonnet (bonnetst@utc.fr)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef IMX_SERIAL_H_
#define IMX_SERIAL_H_

#include <stdint.h>

/* Initialise le port série à la vitesse passée en paramètre (bits/s),
 * 8 bits de données, pas de parité, 1 bit de stop
 */
void serial_init(int baudrate);

/* serial_outbyte
 * Emet l'octet passé en entrée */
void serial_outbyte(uint8_t b);

/* serial_inbyte
 * Retourne un octet lu sur le port série. Bloque si aucun caractère
 * n'a été reçu.
 */
uint8_t serial_inbyte(void);

#endif /* IMX_SERIAL_H_ */
