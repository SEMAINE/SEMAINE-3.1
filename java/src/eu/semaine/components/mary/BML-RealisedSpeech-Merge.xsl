<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:mary="http://mary.dfki.de/2002/MaryXML"
xmlns:bml="http://www.mindmakers.org/projects/BML"
xmlns:ssml="http://www.w3.org/2001/10/synthesis"
exclude-result-prefixes="bml mary #default"
>

<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>
<xsl:strip-space elements="*|text()"/>

<!-- load the merge file -->
<xsl:variable name="emps"
select="document('semaine.mary.realised.acoustics')"/>


  <xsl:template match="@*|node()">
    <xsl:copy>
       <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>


  <xsl:template match="bml:bml/bml:speech">
    <speech> 
	<xsl:copy-of select="@id"/>  
	<xsl:copy-of select="@type"/> 
	<xsl:copy-of select="@voice"/> 
	<xsl:copy-of select="@language"/>
        <xsl:copy-of select="@text"/> 
	<xsl:copy-of select="@speed_sym"/> 

	<!-- Converting MARY realised acoustic params into SEMAINE dataformat-->
        <xsl:apply-templates select="$emps/descendant::*"/>

	<!-- Calculating pitchaccent and timing of center of pitch accented syllable-->	
	<xsl:for-each select="bml:pitchaccent">
             <xsl:variable name="accentid"><xsl:value-of select="@id"/></xsl:variable>  
	     <xsl:variable name="stimeMark"><xsl:value-of select="@start"/></xsl:variable>
	     <xsl:variable name="etimeMark"><xsl:value-of select="@end"/></xsl:variable>
		<xsl:for-each select="$emps/descendant::mary:mark">
			<xsl:if test="@name=$stimeMark">
			    <xsl:variable name="data" select="following::mary:t[1]/child::*"/>
			    <xsl:call-template name="printPitchAccent">
                                    <xsl:with-param name="accentId" select="$accentid"/>
                                    <xsl:with-param name="stimeMark" select="$stimeMark"/>
                                    <xsl:with-param name="etimeMark" select="$etimeMark"/>
                                    <xsl:with-param name="data" select="$data"/>
			    </xsl:call-template>	 				
			</xsl:if>
                </xsl:for-each>
        </xsl:for-each>  
    </speech>
  </xsl:template> 

  <!-- Calculating pitchaccent and timing of center of pitch accented syllable-->	
  <xsl:template name="printPitchAccent">
        <xsl:param name="accentId"/>
        <xsl:param name="stimeMark"/>
        <xsl:param name="etimeMark"/>
        <xsl:param name="data"/>
        <xsl:for-each select="$data">
            <xsl:if test="@accent">
                <xsl:variable name="startTT">
		     <xsl:for-each select="child::*">
			<xsl:if test="position()=1">
 		  	        <xsl:value-of select="@end - @d"/> 
			</xsl:if> 
	 	     </xsl:for-each>
                </xsl:variable>
		<xsl:variable name="endTT">
		<xsl:for-each select="child::*">
			<xsl:if test="position()=last()">
 				<xsl:value-of select="@end"/> 
			</xsl:if> 
		</xsl:for-each></xsl:variable>	
		<xsl:variable name="centerAccent" select="($startTT + (($endTT - $startTT) div 2)) div 1000"/>
                <pitchaccent id="{$accentId}" start="{$stimeMark}" end="{$etimeMark}" time="{$centerAccent}"/> 
	    </xsl:if>
        </xsl:for-each> 
  </xsl:template>

  <!-- Extract syllable elements with phone timings in seconds -->
  <xsl:template name="getSyllableData" match="mary:syllable">
         <xsl:choose>
            <xsl:when test="@accent">
                <mary:syllable accent="1">
		<xsl:copy-of select="@stress"/>
        	<xsl:for-each select="*">
         	     <xsl:variable name="duration" select="@d div 1000"/>
     		     <xsl:variable name="endTime" select="@end div 1000"/>
         	     <xsl:variable name="phone"  select="@p"/>
	             <mary:ph d="{$duration}" end="{$endTime}" p="{$phone}"> </mary:ph>
      		</xsl:for-each>
     		</mary:syllable>
	    </xsl:when>
            <xsl:otherwise>
                <mary:syllable>
		<xsl:copy-of select="@stress"/>
        	<xsl:for-each select="*">
         	     <xsl:variable name="duration" select="@d div 1000"/>
     		     <xsl:variable name="endTime" select="@end div 1000"/>
         	     <xsl:variable name="phone"  select="@p"/>
	             <mary:ph d="{$duration}" end="{$endTime}" p="{$phone}"> </mary:ph>
      		</xsl:for-each>
     		</mary:syllable>
            </xsl:otherwise>
         </xsl:choose> 
   </xsl:template>

   <!-- Extract mark elements with thier names --> 	
   <xsl:template name="getMarkData" match="mary:mark">
	<ssml:mark name="{substring-after(@name,':')}"/>
   </xsl:template>

   <!-- Extract boundary elements with thier durations in Milliseconds(?)  --> 	
   <xsl:template name="getBoundaryData" match="mary:boundary">
	<xsl:variable name="endTime" select="@duration div 1000"/>
	<xsl:variable name="countData" select="position()"/>
	<xsl:variable name="boundaryMark" select="following::mary:mark[1]/@name"/>
	<mary:boundary id="b{$countData}" type="HH" start="{$boundaryMark}" end="{$endTime}"/>
   </xsl:template>

   <!-- Extract text only from token elements --> 	
   <xsl:template match="mary:t"> 
	<xsl:copy-of select="text()"/>
   </xsl:template>
   
   <!-- Discard some MARYXML elements--> 	 
   <xsl:template match="mary:mtu|mary:p|mary:voice|mary:phrase|mary:s|mary:maryxml|mary:ph|mary:prosody"/> 
   
</xsl:stylesheet>

