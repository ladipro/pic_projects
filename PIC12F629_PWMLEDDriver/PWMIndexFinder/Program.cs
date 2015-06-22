using System;
using System.Collections.Generic;
using System.Text;

// EEPROM 'timeline' entry format:
// ===============================
//  7   6   5   4   3   2   1   0
//  |  |---------|  |  |---------|
//  |   intensity   |   LED index
//  ignore          on/off
//
// Example: 0x80 - ignore, this cell is empty
//          0x08 - turn LED 0 on, all intensities
//          0x40 - turn LED 0 off at intensity 4

namespace PWMIndexFinder
{
    class Program
    {
        // intensity -> duty cycle map, hand picked
        static int[] DUTY_CYCLE_MAP = { 0, 1, 4, 8, 15, 23, 45 };

        // minimum/maximum length of the cycle we want to explore
        const int MIN_CYCLE_LENGTH = 81;
        const int MAX_CYCLE_LENGTH = 117;
        
        static byte Encode(int led, int intensity, bool on)
        {
            int result = (intensity << 4) | led;
            if (on)
            {
                result |= 0x08;
            }
            return (byte)result;
        }

        static int DecodeLed(byte code)
        {
            return (code & 0x07);
        }

        static void GenerateHeaderFile(byte[] timeline)
        {
            // append sync point data
            byte[] sync_points = new byte[8];
            for (byte i = 0; i < timeline.Length; i++)
            {
                if ((timeline[i] & 0x08) == 0x08)
                {
                    sync_points[DecodeLed(timeline[i])] = i;
                }
            }
            int timeline_length = timeline.Length;
            Array.Resize(ref timeline, timeline_length + sync_points.Length);
            Array.Copy(sync_points, 0, timeline, timeline_length, sync_points.Length);

            Console.WriteLine("// pwm timeline and sync points for 8 LED intensity modulation");
            Console.WriteLine("// this code has been auto-generated");
            Console.WriteLine("#define MODULATION_CYCLE_LENGTH {0}", timeline_length);
            Console.WriteLine();
            for (int i = 0; i < timeline.Length; i += 8)
            {
                Console.Write("__EEPROM_DATA(");
                for (int j = i; j < i + 8; j++)
                {
                    if (j > i)
                    {
                        Console.Write(", ");
                    }
                    if (j < timeline.Length)
                    {
                        Console.Write("0x{0:x2}", timeline[j]);
                    }
                    else
                    {
                        Console.Write("0x00");
                    }
                }
                Console.WriteLine(");");
            }
        }

        static void Main(string[] args)
        {
            for (int length = MIN_CYCLE_LENGTH; length <= MAX_CYCLE_LENGTH; length++)
            {
                byte[] timeline = new byte[length];
                for (int i = 0; i < length; i++)
                {
                    timeline[i] = 0x80;
                }

                for (int led = 0; led < 8; led++)
                {
                    for (int start = 0; start < length; start++)
                    {
                        bool conflict = false;
                        for (int i = 0; i < 7; i++)
                        {
                            int index = (DUTY_CYCLE_MAP[i] + start) % length;
                            if (timeline[index] != 0x80)
                            {
                                conflict = true;
                                break;
                            }
                            timeline[index] = Encode(led, i, i == 0);
                        }
                        if (conflict)
                        {
                            // backtrack
                            for (int i = 0; i < length; i++)
                            {
                                if (DecodeLed(timeline[i]) == led)
                                {
                                    timeline[i] = 0x80;
                                }
                            }
                        }
                        else
                        {
                            if (led == 7)
                            {
                                // yay, found it!
                                GenerateHeaderFile(timeline);
                                return;
                            }
                            break;
                        }
                    }
                }
            }
            Console.WriteLine("No suitable timeline coverage found :(");
        }
    }
}
