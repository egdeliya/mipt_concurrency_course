package com.lab4udf;

import org.apache.hadoop.hive.ql.exec.Description;
import org.apache.hadoop.hive.ql.exec.UDF;


@Description(
        name = "ReverseUDF",
        value = "Returns reversed string",
        extended = "Example:\n" +
                "  SELECT reverse(\'Hadoop\') FROM dummy;\n" +
                "Output: " +
                "  poodaH"
)
public class ReverseUDF extends UDF {
    /**
     * Reversing of a string.
     * @param str input string
     * @return reversed string
     */
    public String evaluate(String str){ //The method MUST be called quite so!
        return new StringBuilder(str).reverse().toString();
    }

    //I can implement few evaluate-methods with
    //different signatures. Hive will chose one of them
    //according to parameters in query.
}
