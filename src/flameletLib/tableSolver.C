#include "tableSolver.H"

tableSolver::tableSolver(size_t zetaIndex)
{
    collectTables(zetaIndex);
}

void tableSolver::collectTables(size_t zetaIndex)
{
    size_t n=0;
    while(true)
    {
        std::ifstream flameletTableFile("tables/Zeta_"+std::to_string(zetaIndex)+"/flameletTable_"+std::to_string(n)+".csv");
        if (!flameletTableFile) break;
        table* tTable = new table(flameletTableFile);
        tables_.push_back(tTable);
        n++;
    }

    tableNum_ = tables_.size();
    nsp_ = tables_[0]->getNsp();
}

void tableSolver::find(double Z, double Yc)
{
    std::vector<double> Ycs(tableNum_-1);
    std::map<double, size_t> YcMap;
    for (size_t i=0; i<tableNum_; i++)
    {
        tables_[i]->find(Z);
        // The last table is an extiguished one
        if (i!=tableNum_-1)
        {
            Ycs[i] = tables_[i]->lookupYc();
            YcMap.insert({Ycs[i], i});
        }
    }
    std::sort(Ycs.begin(), Ycs.end());
    double YcH, YcL;
    if ( (Yc > Ycs[0]) && (Yc < Ycs[tableNum_-2]) )
    {
        for (size_t i=0; i<tableNum_-1; i++)
        {
            if (Ycs[i] > Yc)
            {
                YcH = Ycs[i];
                YcL = Ycs[i-1];
                break;
            }
        }
        weightL_ = (YcH - Yc) / (YcH - YcL);
        weightH_ = (Yc - YcL) / (YcH - YcL);
        positionL_ = YcMap[YcL];
        positionH_ = YcMap[YcH];
    }
    else if (Yc <= Ycs[0])
    {
        weightL_ = (Ycs[0] - Yc) / (Ycs[0] - 0);
        weightH_ = (Yc - 0) / (Ycs[0] - 0);
        positionL_ = tableNum_-1;
        positionH_ = tableNum_-2;
    }
    else
    {
        YcH = Ycs[tableNum_-2];
        YcL = Ycs[tableNum_-2];
        weightL_ = 0.5;
        weightH_ = 0.5;
        positionL_ = YcMap[YcL];
        positionH_ = YcMap[YcH];
    }
}
