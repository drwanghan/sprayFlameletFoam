{
    volScalarField& he = thermo.he();

    const scalarField& ZCells = Z.internalField();
    const scalarField& varZCells = varZ.internalField();
    const scalarField& YcCells = Yc.internalField();
    const scalarField& pCells = p.internalField();

    scalarField& heCells = he.primitiveFieldRef();
    scalarField& TCells = T.primitiveFieldRef();
    scalarField& omegaYcCells = omegaYc.primitiveFieldRef();

    scalar Zeta = 0.0;

    volScalarField Yt(0.0*Y[0]);
    forAll(ZCells, celli)
    {
        Zeta = Foam::sqrt(varZCells[celli]/Foam::max(ZCells[celli]*(1 - ZCells[celli]), SMALL));
        flameletTable.find(ZCells[celli], Zeta, YcCells[celli]);
        heCells[celli] = 0.0;
        TCells[celli] = flameletTable.lookupT();
        omegaYcCells[celli] = flameletTable.lookupOmegaYc();

        forAll(Y, i)
        {
            scalarField& YCells = Y[i].primitiveFieldRef();
            YCells[celli] = flameletTable.lookupY(i);
            heCells[celli] += YCells[celli]*thermo.composition().Hs(i, pCells[celli], TCells[celli]);
            Yt[celli] += Y[i][celli];
        }
    }

    forAll(Z.boundaryField(), patchI)
    {
        const fvPatchScalarField& pYc = Yc.boundaryField()[patchI];
        const fvPatchScalarField& pvarZ = varZ.boundaryField()[patchI];
        const fvPatchScalarField& pZ = Z.boundaryField()[patchI];
        const fvPatchScalarField& pp = p.boundaryField()[patchI];

        fvPatchScalarField& phe = he.boundaryFieldRef()[patchI];
        fvPatchScalarField& pT = T.boundaryFieldRef()[patchI];
        fvPatchScalarField& pYt = Yt.boundaryFieldRef()[patchI];

        forAll(pZ, facei)
        {
            Zeta = Foam::sqrt(pvarZ[facei]/Foam::max(pZ[facei]*(1 - pZ[facei]), SMALL));
            flameletTable.find(pZ[facei], Zeta, pYc[facei]);
            phe[facei] = 0.0;
            pT[facei] = flameletTable.lookupT();

            forAll(Y, i)
            {
                fvPatchScalarField& pY = Y[i].boundaryFieldRef()[patchI];
                pY[facei] = flameletTable.lookupY(i);
                phe[facei] += pY[facei]*thermo.composition().Hs(i, pp[facei], pT[facei]);
                pYt[facei] += pY[facei];
            }
        }
    }

    thermo.correct();
    // radiation->correct();

    Info<< "T gas min/max   " << min(T).value() << ", "
        << max(T).value() << endl;
    Info<< "p min/max   " << min(p).value() << ", "
        << max(p).value() << endl;
    Info<< "Y oxidizer min/max   " << min(Y[oxidizerIndex]).value() << ", "
        << max(Y[oxidizerIndex]).value() << endl;
    Info<< "Sum Y min/max   " << min(Yt).value() << ", "
        << max(Yt).value() << endl;
    Info<< "OmegaYc min/max   " << min(omegaYc).value() << ", "
        << max(omegaYc).value() << endl;
}
