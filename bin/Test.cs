using System;

namespace Tmp
{
    public class Start
    {
        public Start()
        {
            Console.WriteLine("Start::OnLoad");
        }
    }
}
 
public class Test
{
    static public void Main ()
    {
        Console.WriteLine ("Hello Mono World");
    }
    
    public static void OnLoad()
    {
        Tmp.Start Start = new Tmp.Start();
        Console.WriteLine("OnLoad");
    }
}