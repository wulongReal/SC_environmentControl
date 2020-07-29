 /*
  * FreeModbus Libary: RT-Thread Port
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  */

/* ----------------------- System includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "port.h"
/* ----------------------- Variables ----------------------------------------*/
static rt_base_t level;
/* ----------------------- Start implementation -----------------------------*/
void EnterCriticalSection(void)
{
    level = rt_hw_interrupt_disable();
}

void ExitCriticalSection(void)
{
    rt_hw_interrupt_enable(level);
}

