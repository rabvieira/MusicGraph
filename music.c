/*Developed by Renan Vieira and Danilo Carvalho/*

/*igraph library*/
#include <igraph.h>

/*constants*/
#define algorithm 0 //0: chronological approach
                    //1: affinity approach

void print_matrix(igraph_matrix_t *m)
{
    int i, j;
    for (i=0 ; i<igraph_matrix_nrow(m) ; i++)
    {
        for (j=0 ; j<igraph_matrix_ncol(m) ; j++)
        {
            printf("%li\t", (long int)MATRIX(*m, i, j));
        }
        printf("\n");
    }
}

void print_graph(const igraph_t *graph)
{
    FILE *plot;
    plot = fopen("plot.dot","w");
    igraph_write_graph_dot(graph, plot);
}

long int origin(const igraph_matrix_t *music, int line)
{
    long int i, begin=0, target;
    target = (long int)MATRIX(*music, line, 0);
    for(i=line ; i>=0 ; i--)
        if((long int)MATRIX(*music, i, 0) == target)
            begin ++;
    return begin;
}

long int target(const igraph_matrix_t *music, int line)
{
    long int i;
    for(i=line ; i<igraph_matrix_nrow(music) ; i++)
        if((long int)MATRIX(*music, i, 0) != (long int)MATRIX(*music, i+1, 0))
            return i+1;
}

//Should we add a new degree at the txt?
int new_data(igraph_vector_t *v, long int degree, int tam){
    long int i;
    for(i=0 ; i<tam ; i++)
        if((long int) VECTOR(*v)[i]==degree)
            return (0);
    return (1);
}

//Given a degree K, how many vertex have it?
int how_m(igraph_vector_t *v, long int degree, int max_vertex){
    int i, cont=0;
    for(i=0 ; i<max_vertex ; i++)
        if((long int) VECTOR(*v)[i]==degree)
            cont++;
    return(cont);
}

// x-axis degree (K) #vertex (N)
void print_degrees(const igraph_t *graph, int max_vertex)
{
    int i;
    igraph_vector_t dg;
    FILE* outstream;
    outstream = fopen("degree-dist.txt","w");

    igraph_vector_init(&dg, max_vertex);
    igraph_degree(graph, &dg, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
    igraph_vector_sort(&dg);
    for(i=0 ; i<max_vertex ; i++)
        if(new_data(&dg,(long int) VECTOR(dg)[i],i))
            fprintf(outstream,"%li\t%d\n",(long int) VECTOR(dg)[i],how_m(&dg,(long int) VECTOR(dg)[i],max_vertex)); //(K x N)
    fclose(outstream);
    igraph_vector_destroy(&dg);
}

void get_properties(const igraph_t *graph, long int t, int n, int m)
{
    igraph_real_t avg_path, radius, clustering_coef, closeness=0, density;
    igraph_integer_t diameter, girth, maxd;
    int i;
    FILE *outstream;
    outstream = fopen("measures.txt", "w");
    igraph_vector_t res;
    igraph_vector_init(&res, 0);

    igraph_average_path_length(graph, &avg_path, IGRAPH_UNDIRECTED, 1);
    igraph_diameter(graph, &diameter, 0, 0, 0, IGRAPH_UNDIRECTED, 1);
    igraph_transitivity_undirected(graph, &clustering_coef, IGRAPH_TRANSITIVITY_ZERO);
    igraph_closeness(graph, &res, igraph_vss_all(), IGRAPH_ALL, 0, 0);
    igraph_girth(graph, &girth, 0);
    igraph_maxdegree(graph, &maxd, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
    igraph_density(graph, &density, IGRAPH_NO_LOOPS);
    //igraph_radius(graph, &radius, IGRAPH_ALL);

    for (i=0 ; i<igraph_vector_size(&res) ; i++)
        closeness += (float)VECTOR(res)[i];
    closeness /= 10;

    //fprintf(outstream,"%li\t%d\n",(long int) VECTOR(dg)[i],how_m(&dg,(long int) VECTOR(dg)[i],max_vertex));
    fprintf(outstream, "1.Length of composition:\t%li \n2.Total number of nodes:\t%d \n3.Total number of edges:\t%d \n", t, n, m);
    fprintf(outstream, "4.Mean degree:\t\t\t%.3f \n5.Average path length:\t\t%.3f \n6.Network diameter:\t\t%d \n", ((float)m/(float)n), avg_path, diameter);
    fprintf(outstream, "7.Clustering coefficient:\t%.5f \n8.Average closeness centrality:\t%.5f \n", clustering_coef, closeness);
    fprintf(outstream, "9.Girth:\t\t\t%d\n10.Max Degree:\t\t\t%d\n11.Density:\t\t\t%f", girth, maxd, density);
    print_degrees(graph,n);


           /*avg_path = %f\ndiameter = %d\ngirth = %d\nradius = %f\n", t, n, avg_path, diameter, girth, radius);*/
    fclose(outstream);
    igraph_vector_destroy(&res);
}


/*
    i=0;
    do
    {
        printf("\n%li\n", (long int) VECTOR(*pVector)[i]); //print its neighboors
        i++;
    }while((long int) VECTOR(*pVector)[i] != 0);
*/
int pick_neightboor(igraph_vector_int_t * pVector)
{
    int i=0;
    do
    {
        //printf("\n%li\n", (long int) VECTOR(*pVector)[i]); //print its neighboors
        i++;
    }while((long int) VECTOR(*pVector)[i] != 0);
    int rand_ = igraph_rng_get_integer(igraph_rng_default(), 0, i-1);
    int res = (int)VECTOR(*pVector)[rand_];
    return res;
}

int check_neig(igraph_vector_int_t * pVector)
{
    int i=0;
    do
    {
        i++;
    }while((long int) VECTOR(*pVector)[i] != 0);
    return (int)VECTOR(*pVector)[i-1];
}

int compose_music(const igraph_t *graph, const igraph_matrix_t *music)
{
//------------------------------------------------------------------------
// compose 1... does not respect tempo
//------------------------------------------------------------------------
    int i, next=0;
    int init = igraph_rng_get_integer(igraph_rng_default(), 0, igraph_matrix_nrow(music)-1);
    FILE *outstream;
    //outstream = fopen("composed1.txt", "w");
/*
    for(i=0 ; i<igraph_matrix_nrow(music) ; i++)
    {
        //fprintf(outstream,"%d,%d\n", (int)MATRIX(*music,init+1,0) - (int)MATRIX(*music,init,0), (int)MATRIX(*music,init,1));
        fprintf(outstream,"%d\t%d,%d\n", init, (int)MATRIX(*music,init,0), (int)MATRIX(*music,init,1));
        do
        {
            next = igraph_rng_get_integer(igraph_rng_default(), init-1, init+1);
        }while(next == init || next == igraph_matrix_nrow(music));
        init = next;
    }
*/
    //fclose(outstream);
//------------------------------------------------------------------------
/*
    igraph_adjlist_t list;
    igraph_vector_int_t * pVector;
    igraph_adjlist_init(graph, &list, IGRAPH_ALL);

    pVector = igraph_adjlist_get(&list, 2); //given the id's vertex

    i=0;
    do
    {
        printf("\n%li\n", (long int) VECTOR(*pVector)[i]); //print its neighboors
        i++;
    }while((long int) VECTOR(*pVector)[i] != 0);

    igraph_adjlist_destroy(&list);
*/
//------------------------------------------------------------------------
// compose 2... simple tempo
//------------------------------------------------------------------------
///*
    igraph_adjlist_t list2;
    igraph_vector_int_t * pVector2;
    igraph_adjlist_init(graph, &list2, IGRAPH_ALL);
    int sum2 = 0;
    init = igraph_rng_get_integer(igraph_rng_default(), 0, igraph_matrix_nrow(music)-1);
    outstream = fopen("composed_simple.txt", "w");
    for(i=0 ; i<igraph_matrix_nrow(music) ; i++)
    {
        pVector2 = igraph_adjlist_get(&list2, init);
        int larger_neig2 = check_neig(pVector2);
        fprintf(outstream,"%d,%d\n", sum2 + ((int)MATRIX(*music,larger_neig2,0) - (int)MATRIX(*music,init,0)), (int)MATRIX(*music,init,1));
        sum2 += (int)MATRIX(*music,larger_neig2,0) - (int)MATRIX(*music,init,0);
        do
        {
            next = pick_neightboor(pVector2);
        }while(next == init || next == igraph_matrix_nrow(music));
        //printf("\n%d\n",init);
        init = next;
    }
    fclose(outstream);
    igraph_adjlist_destroy(&list2);
//*/
//------------------------------------------------------------------------
// compose 3 multiple tempo
//------------------------------------------------------------------------
    igraph_adjlist_t list;
    igraph_vector_int_t * pVector;
    igraph_adjlist_init(graph, &list, IGRAPH_ALL);
    int sum = 0;
    int rand_, tempo = 0;
    init = igraph_rng_get_integer(igraph_rng_default(), 0, igraph_matrix_nrow(music)-1);
    outstream = fopen("composed_multiple.txt", "w");
    for(i=0 ; i<igraph_matrix_nrow(music) ; i++)
    {
        pVector = igraph_adjlist_get(&list, init);
        int larger_neig = check_neig(pVector);
        if(!i)
            tempo = 0;
        else
            tempo += (int)MATRIX(*music,larger_neig,0) - (int)MATRIX(*music,init,0);

        rand_ = igraph_rng_get_integer(igraph_rng_default(), 0, 4);
        if(rand_ % 4 == 0 && i>0)
            sum = -1* ((int)MATRIX(*music,larger_neig,0) - (int)MATRIX(*music,init,0));
        else
            sum = 0;

        fprintf(outstream,"%d,%d\n", tempo + sum, (int)MATRIX(*music,init,1));

        do
        {
            next = pick_neightboor(pVector);
        }while(next == init || next == igraph_matrix_nrow(music));
        //printf("\n%d\n",init);
        init = next;
    }
    fclose(outstream);
    igraph_adjlist_destroy(&list);
//------------------------------------------------------------------------

}

int create_graph(char title[], int l, int c)
{
    igraph_t graph;
    igraph_matrix_t music;
    igraph_vector_t vertices;
    FILE *instream;
    int data, i, j;

    igraph_matrix_init(&music, l, c);
    igraph_vector_init(&vertices, l);
    instream = fopen(title,"r");

    if (instream == NULL)
    {
        printf("Error opening the input file\n");
        return (-1);
    }

    for(i=0 ; i<l ; i++)
    {
        for(j=0 ; j<c ; j++)
        {
            fscanf(instream,"%d,", &data);
            MATRIX(music, i, j) = data;
        }
    }

    //print_matrix(&music);
    fclose(instream);

    int additional_edges = 0;
    if(!algorithm)
    {
        igraph_real_t edges[2*l -2]; /* 0,1 0,2 ...*/
        igraph_real_t edges_saved[l];
        for(i=0 ; i<l-1 ; i++)
        {
            if((long int)MATRIX(music, i, 0) != (long int)MATRIX(music, i+1, 0))
            {
               //edges[i] = (int)((float)i/2 + 0.5);
                edges[2*i] = i;
                edges[2*i+1] = i+1;
            }
            else
            {
                edges[2*i] = i- origin(&music,i);
                edges[2*i+1] = i+1;
                edges_saved[additional_edges] = i;
                additional_edges ++;
                //printf("\n%d\n",i);
            }
            //printf("\n%d\n",i);
            //edges[i+1] = ;
        }
        igraph_real_t edges_plus[additional_edges*2];
        for(i=0 ; i<additional_edges ; i++)
        {
            edges_plus[2*i] = edges_saved[i];
            edges_plus[2*i+1] = target(&music,edges_saved[i]);
        }
/*
        for(i=0 ; i<additional_edges*2 ; i++)
        {
            printf(" %f ", edges_plus[i]);
            //printf(" %f ", edges_saved[i]);
        }
*/
        igraph_real_t vextex[(2*l -2) + additional_edges*2];
        for(i=0 ; i<(2*l -2) ; i++)
            vextex[i] = edges[i];
        j=0;
        for(i=2*l -2 ; i<(2*l -2) + additional_edges*2 ; i++)
            vextex[i] = edges_plus[j++];

///*
        for(i=0 ; i<(2*l -2) + additional_edges*2 ; i++)
        {
            //printf("%.0f ", vextex[i]);
            if(vextex[i]<0)
                printf("%.0f ",vextex[i]);
            //printf(" %f ", edges_saved[i]);
        }
//*/

        igraph_vector_view(&vertices, vextex, sizeof(vextex)/sizeof(double));
        //return 0;
        igraph_create(&graph, &vertices, 0, IGRAPH_UNDIRECTED);
        print_graph(&graph);

    }
    else //affinity approach...
    {
        igraph_real_t notes[l];

    }



    get_properties(&graph,(long int)MATRIX(music, l-1, 0),l, ((2*l -2) + additional_edges*2)/2);

    compose_music(&graph, &music);


    igraph_destroy(&graph);
    //igraph_vector_destroy(&vertices);
    igraph_matrix_destroy(&music);
    return (0);
}

int main(void)
{
    igraph_rng_seed(igraph_rng_default(), time(NULL));
    create_graph("danoninho.txt",   //name
                 11,                //lines
                 2);                //columns

    return (0);
}
