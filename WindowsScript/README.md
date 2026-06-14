# Windows Store packaging

Run the packaging script from the repository root:

```powershell
.\WindowsScript\MakeWindowsStoreBundleFile.ps1 -Version 1.0.33.0
```

The script:

1. Configures and builds the `x64-Release` CMake preset.
2. Stages the executable and all deployed runtime files.
3. Creates an x64 `.appx`.
4. Creates an `.appxbundle`.
5. Adds available PDB files as an optional `.appxsym`.
6. Creates the Microsoft Store `.appxupload`.

Results are written below `WindowsScript\Artifacts\<version>`.

Each result folder also contains `TestInstall.ps1` and a public test
certificate. Run the installer to request administrator permission, trust the
certificate in the local computer's `TrustedPeople` store, and install the
signed bundle:

```powershell
.\WindowsScript\Artifacts\1.0.33.0\TestInstall.ps1
```

When no PFX is supplied, the packaging script creates a temporary code-signing
certificate, signs the APPX and bundle, exports the public certificate, and
then deletes the temporary private certificate and PFX.

Package versions must contain four numeric parts, and the final part must be
zero for Microsoft Store submissions.

For a locally installable signed bundle, provide a PFX whose subject matches
the Publisher in `Package.appxmanifest`:

```powershell
.\WindowsScript\MakeWindowsStoreBundleFile.ps1 `
  -Version 1.0.33.0 `
  -CertificatePath C:\path\SkipAdClicker.pfx `
  -CertificatePassword 'password'
```
