#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

// Størrelsen af den reserverede hukommelse i bytes
#define RESERVED_MEMORY_SIZE (100 * 1024 * 1024) // 100 MB

// EFI-thread-procedure
VOID EFIAPI ParallelThread(VOID *Parameter)
{
    // Loop uendeligt
    while (TRUE)
    {
        // Udfør handlinger eller kode her
        // ...

        // Pause for at undgå overdreven CPU-brug
        gBS->Stall(1000000); // Vent 1 millisekund
    }
}

// EFI-applikationsindgangspunkt
EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;

    // create a separate multi1 thread process
    EFI_TPL PreviousTPL;
    Status = gBS->RaiseTPL(TPL_HIGH_LEVEL, &PreviousTPL); // Hæv prioritet for at oprette tråden
    if (EFI_ERROR(Status))
    {
        Print(L"Fejl: Kunne ikke hæve TPL-niveauet\n");
        return Status;
    }

    EFI_HANDLE ThreadHandle;
    Status = gBS->CreateThread(TPL_CALLBACK, ParallelThread, NULL, NULL, &ThreadHandle);
    if (EFI_ERROR(Status))
    {
        Print(L"Fejl: Kunne ikke oprette den parallelle tråd\n");
        return Status;
    }

    // Allocate 100 memory
    EFI_PHYSICAL_ADDRESS ReservedMemory;
    Status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, EFI_SIZE_TO_PAGES(RESERVED_MEMORY_SIZE), &ReservedMemory);
    if (EFI_ERROR(Status))
    {
        Print(L"Fejl: Kunne ikke reservere hukommelse\n");
        return Status;
    }

    // Sænk prioritet tilbage til det oprindelige niveau
    gBS->RestoreTPL(PreviousTPL);

    // Start den indlæste EFI-fil
    Status = gBS->StartImage(ImageHandle, NULL, NULL);
    if (EFI_ERROR(Status))
    {
        Print(L"Fejl: Kunne ikke starte EFI-filen\n");
        return Status;
    }

    // Fortsæt med hovedprogrammet her
    // Afslut hovedprogrammet ved at vente på tastetryk eller lignende
    WaitForSingleEvent(gST->ConIn->WaitForKey, 0);

    // Afslut og ryd op
    gBS->FreePages(ReservedMemory, EFI_SIZE_TO_PAGES(RESERVED_MEMORY_SIZE));

    return EFI_SUCCESS;
}