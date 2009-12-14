<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns="http://mary.dfki.de/2002/MaryXML"
xmlns:mary="http://mary.dfki.de/2002/MaryXML"
xmlns:bml="http://www.mindmakers.org/projects/BML"
xmlns:ssml="http://www.w3.org/2001/10/synthesis">
 
<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>
<xsl:strip-space elements="ssml:*|ssml/text()"/>

  <!--xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template-->

   <xsl:template match="/bml:bml/bml:speech">
     <xsl:param name="character.voice">prudence</xsl:param>
      <maryxml xml:lang="en-GB" version="4">
            <p>
      		<voice name="{$character.voice}">
      			<xsl:apply-templates select="descendant::*"/>
      		</voice>
	  		</p>
      </maryxml>
    </xsl:template>
    
    <xsl:template match="mary:vocalization">
    	<vocalization>
    		<xsl:copy-of select="@meaning"/>  
			<xsl:copy-of select="@name"/> 
			<xsl:copy-of select="@intonation"/>
			<xsl:copy-of select="@voicequality"/>
    	</vocalization>
	</xsl:template>
	
   <xsl:template match="text()"></xsl:template>

</xsl:stylesheet>

