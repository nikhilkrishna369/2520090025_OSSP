#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int source, destination;
    char buffer[100];
    int bytesRead;

    // Open source file
    source = open("source.txt", O_RDONLY);

    // Open destination file
    destination = open("destination.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Copy contents from source to destination
    while ((bytesRead = read(source, buffer, 100)) > 0)
    {
        write(destination, buffer, bytesRead);
    }

    // Close both files
    close(source);
    close(destination);

    printf("File copied successfully.\n");

    return 0;
}
