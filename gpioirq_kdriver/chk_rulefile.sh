#!/bin/bash
# check udev rule file is installed
UDEVRULE_DIR="/etc/udev/rules.d"
RULE_SRCDIR="./udev.rules"
RULE_FILE="gpioirq.rules"
if [ -f "${UDEVRULE_DIR}/${RULE_FILE}" ]; then
  echo "udev rule ${UDEVRULE_DIR}/${RULE_FILE} exist!"
else
  echo "copy rule (cp ${RULE_SRCDIR}/${RULE_FILE} ${UDEVRULE_DIR}/${RULE_FILE} ) ..."
  cp ${RULE_SRCDIR}/${RULE_FILE}    ${UDEVRULE_DIR}/${RULE_FILE}
fi


