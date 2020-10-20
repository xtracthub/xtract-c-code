#include "2_helper.c"

int albums_command(Artist art, Lists m_list);
int artists_command(Artist art, Lists m_list);
int songs_command(Artist art, Lists m_list);
int querying(char *input, Lists m_list);

int main()
{
    Lists master_lists = load();

    printf("\n** Welcome to Cotify -- the CS152 music information service. **");
    printf("\n\nCommands to learn more about our music:\n");
    printf("  albums [artist]\n");
    printf("  songs [artist]\n");
    printf("  artists [artist]\n");
    printf("Please put quotes around the artist's name.\n");
    printf("To exit the program, type \"close\"\n\n");
    printf("Note: currently we feature information about The Beatles, Queen, ");
    printf("and fun.\n\n");

    int end_program = 0;
    char *input;
    input = (char *) calloc(23, sizeof(char));

    while (!end_program)
    {
        fgets(input, 23, stdin);

        if (strncmp(input, "close", 5) == 0)
        {
            end_program = 1;
            continue;
        }

        querying(input, master_lists);
        printf("\nPlease enter another selection:\n\n");
    }

    free(input);
    printf("** Thank you for using Cotify! Goodbye! **\n");
    return 0;
}

/**
 * This function executes the albums command; it prints all albums by the
 * given artist.
 * PARAMETERS: the artist; master list to search through.
 * RETURNS: 0, upon completion.
 */
int albums_command(Artist art, Lists m_list)
{
    printf("%s has created ", art.name);
    int n = 1; // album counter
    for (int i = 0; i < sizeof(m_list.albums) / sizeof(m_list.albums[0]); i++)
    { // loop through albums
        if (strncmp(m_list.albums[i].artist.name, art.name, art.name_len) == 0)
        {
            if (n > 1)
            {
                printf(" and ");
            }
            printf("%s (%d)", m_list.albums[i].name, m_list.albums[i].release_year);
            n++;
        }
    }
    printf(".\n");
    return 0;
}

/**
 * This function executes the artists command; it prints the artist's name,
 * birth year, and number of songs on Cotify.
 * PARAMETERS: the artist; master list to search through.
 * RETURNS: 0, upon completion.
 */
int artists_command(Artist art, Lists m_list)
{
    printf("Artist: %s -- Born: %d -- Number of songs in Cotify: %d\n",
           art.name, art.birth_year, num_songs(art, m_list));
    return 0;
}

/**
 * This function executes the songs command; it prints all songs by the
 * given artist.
 * PARAMETERS: the artist; master list to search through.
 * RETURNS: 0, upon completion.
 */
int songs_command(Artist art, Lists m_list)
{
    for (int i = 0; i < sizeof(m_list.songs) / sizeof(m_list.songs[0]); i++)
    { // loop through all songs
        if (strncmp(m_list.songs[i].artist.name, art.name, art.name_len) == 0)
        { // if the song's artist matches the given artist
            printf("Song: %s -- %d seconds -- Artist: %s -- Album: %s\n",
                   m_list.songs[i].title, m_list.songs[i].length,
                   m_list.songs[i].artist.name,
                   find_album(m_list.songs[i], m_list).name);
        }
    }
    return 0;
}

/**
 * This function looks at the user's input, parses it into a command string
 * and an artist string, and either prints an "error" or redirects to a new
 * function to actually execute the command.
 * PARAMETERS: (pointer to) user's input; master list to pass to other funcs.
 * RETURNS: 0, upon completion.
 */
int querying(char *input, Lists m_list)
{
    /* Part 1: Parsing command and artist name */

    char *ptr = input;
    char command[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char artist_input[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int shift = 0;
    while ((int) *(ptr + shift) != 32 && shift < 8) // 32 is space character
    {
        command[shift] = *(ptr + shift);
        shift++;
    }
    if (*(ptr + (++shift)) != 34)
    {
        printf("You did not use quotation marks.\n");
        return 0;
    }
    shift++; // go just after first quotation mark
    int artist_index = 0;
    while ((int) *(ptr + shift) != 34) // 34 is ascii code for quotation mark
    {
        artist_input[artist_index] = *(ptr + shift);
        shift++;
        artist_index++;
    }

    /* Part 2: Error or transition to new function */

    Artist art = match_artist(artist_input, m_list);

    if (strncmp(art.name, "no name", 7) == 0)
    { // if artist input wasn't valid
        printf("Invalid input: artist not found.\n");
        return 0;
    }

    // in my infinite kindness, you don't actually have to type the final s.
    if (strncmp(command, "albums", 5) == 0)
    {
        albums_command(art, m_list);
    }
    else if (strncmp(command, "songs", 4) == 0)
    {
        songs_command(art, m_list);
    }
    else if (strncmp(command, "artists", 6) == 0)
    {
        artists_command(art, m_list);
    }
    else
    {
        printf("Invalid input: command not found.\n");
    }
    return 0;
}
