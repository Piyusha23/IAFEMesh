/* Color-fills an arbitrarily-shaped polygon described by VertexList.
   If the first and last points in VertexList are not the same, the path
   around the polygon is automatically closed. Returns 1 for success, 
   0 if memory allocation failed.
   If the polygon shape is known in advance, speedier processing may be
   enabled by specifying the shape as follows: "convex" - a rubber band
   stretched around the polygon would touch every vertex in order;
   "nonconvex" - the polygon is not self-intersecting, but need not be
   convex; "complex" - the polygon may be self-intersecting, or, indeed,
   any sort of polygon at all. Complex will work for all polygons; convex
   is fastest. Undefined results will occur if convex is specified for a
   nonconvex or complex polygon.
   Define CONVEX_CODE_LINKED if the fast convex polygon filling code from
   Chapter 21 is linked in. Otherwise, convex polygons are
   handled by the complex polygon filling code.
   Nonconvex is handled as complex in this implementation. See text for a
   discussion of faster nonconvex handling

   Tested with Borland C++ 4.02 by Jim Mischel 12/16/94.
*/

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "PolygonFill.h"

#define SWAP(a,b) {temp = a; a = b; b = temp;}

namespace PolygonFill
{

  struct EdgeState 
  {
    struct EdgeState *NextEdge;
    int X;
    int StartY;
    int WholePixelXMove;
    int XDirection;
    int ErrorTerm;
    int ErrorTermAdjUp;
    int ErrorTermAdjDown;
    int Count;
  };

  /* Pointers to global edge table (GET) and active edge table (AET) */
  static struct EdgeState *GETPtr, *AETPtr;
  static LineDraw *theLineDraw;

  /* Creates a GET in the buffer pointed to by NextFreeEdgeStruc from
     the vertex list. Edge endpoints are flipped, if necessary, to
     guarantee all edges go top to bottom. The GET is sorted primarily
     by ascending Y start coordinate, and secondarily by ascending X
     start coordinate within edges with common Y coordinates */
  static void BuildGET(PointListHeader * VertexList,
                       struct EdgeState * NextFreeEdgeStruc,
                       struct EdgeState *LastFreeEdgeStruc)
  {
    int i, StartX, StartY, EndX, EndY, DeltaY, DeltaX, Width, temp;
    struct EdgeState *NewEdgePtr;
    struct EdgeState *FollowingEdge, **FollowingEdgeLink;
    Point *VertexPtr;

    /* Scan through the vertex list and put all non-0-height edges into
       the GET, sorted by increasing Y start coordinate */
    VertexPtr = VertexList->PointPtr;   /* point to the vertex list */
    GETPtr = NULL;    /* initialize the global edge table to empty */
    for (i = 0; i < VertexList->Length; i++)
      {
      /* Calculate the edge height and width */
      StartX = VertexPtr[i].X;
      StartY = VertexPtr[i].Y;
      /* The edge runs from the current point to the previous one */
      if (i == 0)
        {
        /* Wrap back around to the end of the list */
        EndX = VertexPtr[VertexList->Length-1].X;
        EndY = VertexPtr[VertexList->Length-1].Y;
        }
      else
        {
        EndX = VertexPtr[i-1].X;
        EndY = VertexPtr[i-1].Y;
        }
      /* Make sure the edge runs top to bottom */
      if (StartY > EndY)
        {
        SWAP(StartX, EndX);
        SWAP(StartY, EndY);
        }
      /* Skip if this can't ever be an active edge (has 0 height) */
      if ((DeltaY = EndY - StartY) != 0)
        {
        /* Allocate space for this edge's info, and fill in the
           structure */
        NewEdgePtr = NextFreeEdgeStruc++;
        if(NewEdgePtr == LastFreeEdgeStruc)
          {
          throw;
          }
        NewEdgePtr->XDirection =   /* direction in which X moves */
          ((DeltaX = EndX - StartX) > 0) ? 1 : -1;
        Width = abs(DeltaX);
        NewEdgePtr->X = StartX;
        NewEdgePtr->StartY = StartY;
        NewEdgePtr->Count = DeltaY;
        NewEdgePtr->ErrorTermAdjDown = DeltaY;
        if (DeltaX >= 0)  /* initial error term going L->R */
          NewEdgePtr->ErrorTerm = 0;
        else              /* initial error term going R->L */
          NewEdgePtr->ErrorTerm = -DeltaY + 1;
        if (DeltaY >= Width)
          {     /* Y-major edge */
          NewEdgePtr->WholePixelXMove = 0;
          NewEdgePtr->ErrorTermAdjUp = Width;
          }
        else
          {                   /* X-major edge */
          NewEdgePtr->WholePixelXMove =
            (Width / DeltaY) * NewEdgePtr->XDirection;
          NewEdgePtr->ErrorTermAdjUp = Width % DeltaY;
          }
        /* Link the new edge into the GET so that the edge list is
           still sorted by Y coordinate, and by X coordinate for all
           edges with the same Y coordinate */
        FollowingEdgeLink = &GETPtr;
        for (;;)
          {
          FollowingEdge = *FollowingEdgeLink;
          if ((FollowingEdge == NULL) ||
              (FollowingEdge->StartY > StartY) ||
              ((FollowingEdge->StartY == StartY) &&
               (FollowingEdge->X >= StartX)))
            {
            NewEdgePtr->NextEdge = FollowingEdge;
            *FollowingEdgeLink = NewEdgePtr;
            break;
            }
          FollowingEdgeLink = &FollowingEdge->NextEdge;
          }
        }
      }
  }

  /* Moves all edges that start at the specified Y coordinate from the
     GET to the AET, maintaining the X sorting of the AET. */
  static void MoveXSortedToAET(int YToMove)
  {
    struct EdgeState *AETEdge, **AETEdgePtr, *TempEdge;
    int CurrentX;

    /* The GET is Y sorted. Any edges that start at the desired Y
       coordinate will be first in the GET, so we'll move edges from
       the GET to AET until the first edge left in the GET is no longer
       at the desired Y coordinate. Also, the GET is X sorted within
       each Y coordinate, so each successive edge we add to the AET is
       guaranteed to belong later in the AET than the one just added */
    AETEdgePtr = &AETPtr;
    while ((GETPtr != NULL) && (GETPtr->StartY == YToMove))
      {
      CurrentX = GETPtr->X;
      /* Link the new edge into the AET so that the AET is still
         sorted by X coordinate */
      for (;;)
        {
        AETEdge = *AETEdgePtr;
        if ((AETEdge == NULL) || (AETEdge->X >= CurrentX))
          {
          TempEdge = GETPtr->NextEdge;
          *AETEdgePtr = GETPtr;  /* link the edge into the AET */
          GETPtr->NextEdge = AETEdge;
          AETEdgePtr = &GETPtr->NextEdge;
          GETPtr = TempEdge;   /* unlink the edge from the GET */
          break;
          }
        else
          {
          AETEdgePtr = &AETEdge->NextEdge;
          }
        }
      }
  }
  /* Fills the scan line described by the current AET at the specified Y
     coordinate in the specified color, using the odd/even fill rule */
  static void ScanOutAET(int YToScan)
  {
    int LeftX;
    struct EdgeState *CurrentEdge;

    /* Scan through the AET, drawing line segments as each pair of edge
       crossings is encountered. The nearest pixel on or to the right
       of left edges is drawn, and the nearest pixel to the left of but
       not on right edges is drawn */
    CurrentEdge = AETPtr;
    while (CurrentEdge != NULL)
      {
      LeftX = CurrentEdge->X;
      CurrentEdge = CurrentEdge->NextEdge;
      theLineDraw->Draw(YToScan,LeftX,CurrentEdge->X-1);
      CurrentEdge = CurrentEdge->NextEdge;
      }
  }

  /* Advances each edge in the AET by one scan line.
     Removes edges that have been fully scanned. */
  static void AdvanceAET()
  {
    struct EdgeState *CurrentEdge, **CurrentEdgePtr;

    /* Count down and remove or advance each edge in the AET */
    CurrentEdgePtr = &AETPtr;
    while ((CurrentEdge = *CurrentEdgePtr) != NULL)
      {
      /* Count off one scan line for this edge */
      if ((--(CurrentEdge->Count)) == 0)
        {
        /* This edge is finished, so remove it from the AET */
        *CurrentEdgePtr = CurrentEdge->NextEdge;
        }
      else
        {
        /* Advance the edge's X coordinate by minimum move */
        CurrentEdge->X += CurrentEdge->WholePixelXMove;
        /* Determine whether it's time for X to advance one extra */
        if ((CurrentEdge->ErrorTerm +=
             CurrentEdge->ErrorTermAdjUp) > 0)
          {
          CurrentEdge->X += CurrentEdge->XDirection;
          CurrentEdge->ErrorTerm -= CurrentEdge->ErrorTermAdjDown;
          }
        CurrentEdgePtr = &CurrentEdge->NextEdge;
        }
      }
  }
  /* Sorts all edges currently in the active edge table into ascending
     order of current X coordinates */
  static void XSortAET()
  {
    struct EdgeState *CurrentEdge, **CurrentEdgePtr, *TempEdge;

    /* Scan through the AET and swap any adjacent edges for which the
       second edge is at a lower current X coord than the first edge.
       Repeat until no further swapping is needed */
    if (AETPtr != NULL)
      {
      int SwapOccurred;
      do
        {
        SwapOccurred = 0;
        CurrentEdgePtr = &AETPtr;
        while ((CurrentEdge = *CurrentEdgePtr)->NextEdge != NULL)
          {
          if (CurrentEdge->X > CurrentEdge->NextEdge->X)
            {
            /* The second edge has a lower X than the first;
               swap them in the AET */
            TempEdge = CurrentEdge->NextEdge->NextEdge;
            *CurrentEdgePtr = CurrentEdge->NextEdge;
            CurrentEdge->NextEdge->NextEdge = CurrentEdge;
            CurrentEdge->NextEdge = TempEdge;
            SwapOccurred = 1;
            }
          CurrentEdgePtr = &(*CurrentEdgePtr)->NextEdge;
          }
        }
      while (SwapOccurred != 0);
      }
  }


  int Fill(PointListHeader * VertexList,
           LineDraw *drawer)
  {
    struct EdgeState *EdgeTableBuffer;
    int CurrentY;
    theLineDraw = drawer;

    /* It takes a minimum of 3 vertices to cause any pixels to be
       drawn; reject polygons that are guaranteed to be invisible */
    if (VertexList->Length < 3)
      return(1);
    /* Get enough memory to store the entire edge table */
    if ((EdgeTableBuffer = new EdgeState[VertexList->Length+1]) == 0)
      {
      return(0);  /* couldn't get memory for the edge table */
      }
    EdgeState *last = EdgeTableBuffer+VertexList->Length;
    last->X = last->StartY = last->WholePixelXMove =
      last->XDirection = last->ErrorTerm = last->ErrorTermAdjUp =
      last->ErrorTermAdjDown = last->Count = 0xDEADBEEF;
  
    /* Build the global edge table */
    BuildGET(VertexList, EdgeTableBuffer,EdgeTableBuffer+VertexList->Length);
    /* Scan down through the polygon edges, one scan line at a time,
       so long as at least one edge remains in either the GET or AET */
    AETPtr = NULL;    /* initialize the active edge table to empty */
    CurrentY = GETPtr->StartY; /* start at the top polygon vertex */
    while ((GETPtr != NULL) || (AETPtr != NULL))
      {
      MoveXSortedToAET(CurrentY);  /* update AET for this scan line */
      ScanOutAET(CurrentY); /* draw this scan line from AET */
      AdvanceAET();                /* advance AET edges 1 scan line */
      XSortAET();                  /* resort on X */
      CurrentY++;                  /* advance to the next scan line */
      }
    /* Release the memory we've allocated and we're done */
    delete [] EdgeTableBuffer;//  free(EdgeTableBuffer);
    return(1);
  }

}
