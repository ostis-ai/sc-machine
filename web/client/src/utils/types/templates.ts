import { ScAddr } from "@ostis/sc-core";

export interface KBTemplate {
  addr: ScAddr,
  scsContent: string,
  title: string,
}