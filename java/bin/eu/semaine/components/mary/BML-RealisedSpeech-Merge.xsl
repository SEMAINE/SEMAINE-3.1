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


  <xsl:template match="bml/speech">
    <speech> 
	<xsl:copy-of select="@id"/>  
	<xsl:copy-of select="@type"/> 
	<xsl:copy-of select="@voice"/> 
	<xsl:copy-of select="@language"/>
        <xsl:copy-of select="@text"/> 
	<xsl:copy-of select="@speed_sym"/> 

	<xsl:for-each select="ssml:mark">
             <xsl:variable name="id"><xsl:value-of select="@name"/></xsl:variable>  
             <xsl:copy-of select="."/>
             <xsl:apply-templates select="$emps/child::*">
                    <xsl:with-param name="markName" select="$id"/>
             </xsl:apply-templates>
        </xsl:for-each>

	<xsl:for-each select="pitchaccent">
             <xsl:variable name="accentid"><xsl:value-of select="@name"/></xsl:variable>  
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


  <xsl:template name="getToken" match="mary:*">
     <xsl:param name="markName"/>
     <xsl:for-each select="descendant::mary:mark">
          <xsl:if test="$markName=@name">
            <xsl:copy-of select="following::mary:t[1]/text()"/>
	    <xsl:variable name="tokenContent" select="following::mary:t[1]/child::*"/>
            <xsl:apply-templates select="$tokenContent">
            </xsl:apply-templates>
          </xsl:if>
     </xsl:for-each> 
   </xsl:template>

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

 
</xsl:stylesheet>

