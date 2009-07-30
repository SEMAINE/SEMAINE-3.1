<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns="http://www.w3.org/2001/10/synthesis"
xmlns:bml="http://www.mindmakers.org/projects/BML"
xmlns:ssml="http://www.w3.org/2001/10/synthesis">
 
<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>
<xsl:strip-space elements="ssml:*|ssml/text()"/>

  <!--xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template-->

   <xsl:template match="/fml-apml/bml:bml/bml:speech">
     <xsl:param name="character.voice">prudence</xsl:param>
     <speak xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
             xml:lang="{@language}">
      <voice name="{$character.voice}">       
	  <xsl:copy-of select="ssml:*|text()"/>
	  </voice>
      </speak>
    </xsl:template>


	

   <xsl:template match="text()"/>
   

</xsl:stylesheet>
