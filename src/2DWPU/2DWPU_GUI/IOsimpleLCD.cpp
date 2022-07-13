#include "IOsimpleLCD.h"

namespace WPU2D
{
	namespace Core
	{
		IOsimpleLCD::IOsimpleLCD(QGraphicsScene *scene)
		{
			this->scene = scene;
			ClearBuffer();
			Display();
			baseaddr = 0;
		}

		void IOsimpleLCD::Write16(reg16 val, reg16 addr)
		{
			switch(addr-baseaddr)
			{
			case simpleLCD_command:
				if(val == 0)
					ClearBuffer();
				if(val == 1)
					WritePixel();
				if(val == 2)
				{
					Display();
					ClearBuffer();
				}
				break;

			case simpleLCD_writeX:
				x = val;
				break;

			case simpleLCD_writeY:
				y = val;
				break;

			case simpleLCD_writeR:
				r = val;
				break;

			case simpleLCD_writeG:
				g = val;
				break;

			case simpleLCD_writeB:
				b = val;
				break;
			}
		}

		void IOsimpleLCD::ClearBuffer()
		{
			for(int i = 0; i < 400*240*3; ++i)
				buffer[i] = 0;
		}

		void IOsimpleLCD::Display()
		{
			QImage img(buffer, 400, 240, QImage::Format_RGB888);
			scene->clear();
			scene->addPixmap(QPixmap::fromImage(img));
			scene->update();
		}

		void IOsimpleLCD::WritePixel()
		{
			x %= 400;
			y %= 240;

			buffer[ (x + (y*400))*3 + 0] = b;
			buffer[ (x + (y*400))*3 + 1] = g;
			buffer[ (x + (y*400))*3 + 2] = r;
		}
	}
}