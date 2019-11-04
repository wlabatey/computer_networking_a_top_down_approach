public class Packet
{
    private int source;
    private int dest;
    private int[] mincost;
    
    public Packet(Packet p)
    {
        source = p.getSource();
        dest = p.getDest();
        mincost = new int[NetworkSimulator.NUMENTITIES];
        for (int i = 0; i < NetworkSimulator.NUMENTITIES; i++)
        {
            mincost[i] = p.getMincost(i);
        }
    }
    
    public Packet(int s, int d, int[] mc)
    {
        source = s;
        dest = d;
        
        mincost = new int[NetworkSimulator.NUMENTITIES];
        if (mc.length != NetworkSimulator.NUMENTITIES)
        {
            System.out.println("Packet(): Invalid data format.");
            System.exit(1);
        }
        
        for (int i = 0; i < NetworkSimulator.NUMENTITIES; i++)
        {
            mincost[i] = mc[i];
        }
    }        
    
    public int getSource()
    {
        return source;
    }
    
    public int getDest()
    {
        return dest;
    }
    
    public int getMincost(int ent)
    {
        return mincost[ent];
    }

    public String toString()
    {
        String str;
        str = "source: " + source + "  dest: " + dest + "  mincosts: ";
        
        for (int i = 0; i < NetworkSimulator.NUMENTITIES; i++)
        {
            str = str + i + "=" + getMincost(i) + " ";
        }
        
        return str;
        
    }
    
}
