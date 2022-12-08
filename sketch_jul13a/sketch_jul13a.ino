//void printArray ( const int [][ 3 ] ); // prototype
const int rows = 2;
const int columns = 3;
int array1[ rows ][ columns ] = { { 1, 2, 3 }, { 4, 5, 6 } };
//int array2[ rows ][ columns ] = { 1, 2, 3, 4, 5 };
//int array3[ rows ][ columns ] = { { 1, 2 }, { 4 } };
//Serial.println (array1[ 0 ][ 1 ] );
void setup () {
Serial.begin(115200);
}
void loop () {
  Serial.println (array1[ 0 ][ 1 ] );
//   Serial.println ("Values in array1 by row are: ") ;
//   Serial.print ("\r" ) ;
 //  printArray(array1) ;
//   Serial.print ("Values in array2 by row are: ") ;
//   Serial.print ("\r" ) ;
//   printArray(array2) ;
//   Serial.print ("Values in array3 by row are: ") ;
//   Serial.print ("\r" ) ;
//   printArray(array3) ;
   delay(10000);
}

// output array with two rows and three columns

void printArray( const int a[][ columns ] ) {
   // loop through array's rows
   for ( int i = 0; i < rows; ++i ) {
      // loop through columns of current row
      for ( int j = 0; j < columns; ++j )
      Serial.println (a[ i ][ j ] );
      Serial.print ("\r" ) ; // start new line of output
   } 
// end outer for
} 

// end function printArray
